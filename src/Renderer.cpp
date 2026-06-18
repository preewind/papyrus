#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>

#include "Renderer.h"
#include "Editor.h"
#include "FileBrowser.h"
#include "SearchSession.h"
#include "logger.h"
#include "util.h"

Renderer::Renderer(SDL_Window *window)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);
    CSF(SDL_SetRenderVSync(mRenderer, 1)); // TODO maybe make this an option to set manually

    if (!mRenderer)
    {
        throw std::runtime_error("Failed to create renderer");
    }
    if (!TTF_Init())
    {
        throw std::runtime_error("Failed to initialize SDL_ttf");
    }
    mFont = TTF_OpenFont("assets/JetBrainsMono-Regular.ttf", mFontSize);
    if (!mFont)
    {
        throw std::runtime_error("Failed to load font");
    }
    mTextLayout.setFont(mFont);
    // editor layout
    mLayout.lineHeight = getLineHeight();
    SDL_GetWindowSize(window, (int *)&mLayout.windowWidth, (int *)&mLayout.totalWindowHeight);
    mLayout.windowHeight = mLayout.totalWindowHeight;

    mTerminalLayout.windowHeight = 0.25 * mLayout.totalWindowHeight;
    mTerminalLayout.windowX = 0;
    mTerminalLayout.windowY = mLayout.totalWindowHeight - mTerminalLayout.windowHeight;

    // search layout
    mSearchLayout.queryX = mLayout.marginLeft + mLayout.windowWidth / 2;
    mSearchLayout.queryY = mLayout.marginTop + mLayout.lineHeight;
    mSearchLayout.queryWidth = mLayout.windowWidth - mSearchLayout.queryX - mLayout.marginRight - mSearchLayout.matchBoxWidth;
    mSearchLayout.queryHeight = 1.5 * mLayout.lineHeight;
    mSearchLayout.matchBoxX = mSearchLayout.queryX + mSearchLayout.queryWidth + mSearchLayout.boxSpacing;
    mSearchLayout.textX = mSearchLayout.queryX + mSearchLayout.textPadding;
    mSearchLayout.textY = mSearchLayout.queryY + (mSearchLayout.queryHeight - mLayout.lineHeight) / 2; // gives you a vertically centered text
    mSearchLayout.matchBoxTextX = mSearchLayout.matchBoxX + mSearchLayout.textPadding;
}

Renderer::~Renderer()
{
    TTF_CloseFont(mFont);
    TTF_Quit();

    SDL_DestroyRenderer(mRenderer);
}

void Renderer::clear()
{
    CSF(SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255));
    CSF(SDL_RenderClear(mRenderer));
}

int Renderer::getLineHeight() const
{
    return TTF_GetFontHeight(mFont);
}

const EditorLayout &Renderer::getEditorLayout() const
{
    return mLayout;
}

SDL_Color Renderer::getColorFromTokenType(const Token &token)
{
    switch (token.type)
    {
    case TokenType::OpenCurly:
    case TokenType::CloseCurly:
    case TokenType::OpenParen:
    case TokenType::CloseParen:
        return mLexerTheme.Punctuation;
    case TokenType::Comment:
        return mLexerTheme.Comment;
    case TokenType::String:
        return mLexerTheme.String;
    case TokenType::Keyword:
        return mLexerTheme.Keyword;
    case TokenType::Preprocessor:
        return mLexerTheme.Preprocessor;
    case TokenType::IncludeLib:
        return mLexerTheme.IncludeLib;
    default:
        return mLexerTheme.Default;
    }
}

void Renderer::drawText(const std::string &text, int x, int y)
{
    drawText(text, x, y, mTheme.text);
}

void Renderer::drawText(const std::string &text, int x, int y, SDL_Color color)
{
    if (text.empty())
        return;

    SDL_Surface *surface =
        TTF_RenderText_Blended(
            mFont,
            text.c_str(),
            text.size(),
            color);

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(
            mRenderer,
            surface);

    SDL_FRect dst;
    dst.x = static_cast<float>(x);
    dst.y = static_cast<float>(y);
    dst.w = static_cast<float>(surface->w);
    dst.h = static_cast<float>(surface->h);

    CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void Renderer::drawTextTokenized(const std::string &text, uint32_t y, const std::vector<Token> &tokens)
{
    std::string expandedLine = mTextLayout.expandTabs(text);
    for (const Token &token : tokens)
    {
        uint32_t vCol = mTextLayout.virtualColumn(text, token.col);
        const std::string &subs = expandedLine.substr(vCol, token.length);
        int xOffset = mTextLayout.width(std::string(vCol, ' '));
        int renderX = mLayout.marginLeft - mScrollOffsetX + xOffset;
        drawText(subs, renderX, y, getColorFromTokenType(token));
    }
}

void Renderer::drawRect(int x, int y, int w, int h, SDL_Color color)
{
    SDL_FRect rect;
    rect.x = static_cast<float>(x);
    rect.y = static_cast<float>(y);
    rect.w = static_cast<float>(w);
    rect.h = static_cast<float>(h);

    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));

    CSF(SDL_RenderFillRect(mRenderer, &rect));
}

void Renderer::renderLineNumbers(uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows)
{
    uint32_t first = scrollOffsetY;
    uint32_t last = std::min(first + visibleRows, numLines);
    for (uint32_t i = first; i < last; ++i)
    {

        drawText(std::to_string(i + 1), mLayout.lineNumberAreaWidth / 2 - mTextLayout.width(std::to_string(i + 1)) / 2, screenY(i, scrollOffsetY), mTheme.lineNumbers);
    }
}

void Renderer::renderCursor(const Cursor &cursor, const std::string &text, uint32_t offsetY)
{
    if (mCursorBlinker.visible())
    {
        int x = mLayout.marginLeft + mTextLayout.columnToPixel(text, cursor.col) - mScrollOffsetX;
        int y = screenY(cursor.row, offsetY);

        drawRect(x, y, 2, mLayout.lineHeight, mTheme.cursor);
    }
}

void Renderer::renderText(const Editor &editor)
{
    auto &text = editor.getText();
    int visRows = editor.getVisibleRows();
    int first = editor.getScrollOffsetY();
    int last = std::min(
        first + visRows,
        (int)text.size());
    const auto &tokens = editor.getTokens();
    for (int i = first; i < last; ++i)
    {
        if (tokens.size() > 0)
        {
            drawTextTokenized(text[i], screenY(i, first), tokens[i]);
        }
        else
        {
            drawText(mTextLayout.expandTabs(text[i]), mLayout.marginLeft - mScrollOffsetX, screenY(i, first));
        }
    }
}

void Renderer::renderSelection(const Editor &editor)
{
    Selection selection = editor.getSelection().normalized();

    if (selection.empty())
        return;

    const Position &start = selection.begin;
    const Position &end = selection.end;

    for (size_t row = start.row; row <= end.row; ++row)
    {
        if (row < editor.getScrollOffsetY())
            continue;
        if (row >= editor.getScrollOffsetY() + editor.getVisibleRows())
            break;

        int beginCol, endCol;
        if (row == start.row)
        {

            beginCol = start.col;
            // if only one line selected
            if (start.row == end.row)
            {
                endCol = end.col;
            }
            else
            {
                endCol = editor.getLineString(row).size();
            }
        }
        // in between line -> should be fully selected
        else if (row < end.row)
        {
            beginCol = 0;
            endCol = editor.getLineString(row).size();
        }
        else
        {
            beginCol = 0;
            endCol = end.col;
        }
        const std::string &line = editor.getLineString(row);
        renderHighlightedRange(line, row, beginCol, endCol - beginCol, editor.getScrollOffsetY());
    }
}

void Renderer::renderEditor(const Editor &editor)
{
    renderLineNumbers(editor.getLineCount(), editor.getScrollOffsetY(), editor.getVisibleRows());
    SDL_Rect clipRect{
        mLayout.marginLeft,
        0,
        mLayout.windowWidth - mLayout.marginLeft,
        mLayout.windowHeight};

    CSF(SDL_SetRenderClipRect(mRenderer, &clipRect));

    if (editor.getSelectionActive())
    {
        renderSelection(editor);
    }
    if (editor.isSearchActive())
    {
        renderSearchMatches(editor.getSearch(), editor);
    }
    Cursor cursor = editor.getCursor();
    std::string currentLineText = editor.getLineString(cursor.row);
    renderCursor(cursor, currentLineText, editor.getScrollOffsetY());
    renderText(editor);
    if (editor.isSearchActive())
    {
        ensureCursorVisibleHorizontallySearch(editor.getSearch().getCursor(), editor.getSearch().getQuery());
        renderSearchOverlay(editor.getSearch());
        renderSearchCursor(editor.getSearch());
    }
    CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
    if (editor.isTerminalVisible())
    {
        renderTerminal(editor);
    }
}

void Renderer::renderTerminal(const Editor &editor)
{
    drawRect(mTerminalLayout.windowX, mTerminalLayout.windowY, mLayout.windowWidth, mTerminalLayout.windowHeight, mTheme.terminalBackground);
    const Terminal &terminal = editor.getTerminalConst();
    renderTerminalCursor(terminal);
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    std::vector<std::string> output = terminal.getOutput().getText();
    std::reverse(output.begin(), output.end());
    if (output.size() == 0)
        return;
    uint32_t visRows = terminal.getVisibleRows();
    uint32_t first = terminal.getScrollOffset();
    uint32_t last = std::min(visRows, (uint32_t)output.size());
    for (uint32_t i = 0; i < last; ++i)
    {
        drawText(output[first + i], mTerminalLayout.windowX + mTerminalLayout.marginLeft, mLayout.totalWindowHeight - mTerminalLayout.marginTop - (i + 2) * mLayout.lineHeight);
    }

    drawText(text, mTerminalLayout.windowX + mTerminalLayout.marginLeft, mLayout.totalWindowHeight - mTerminalLayout.marginTop - mLayout.lineHeight);
}

void Renderer::renderTerminalCursor(const Terminal &terminal)
{
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    uint32_t cursorTextWidth = mTextLayout.width(text.substr(0, text.size() + terminal.getCursor() - terminal.getInput().size()));
    drawRect(mTerminalLayout.windowX + mTerminalLayout.marginLeft + cursorTextWidth, mLayout.totalWindowHeight - mTerminalLayout.marginTop - mLayout.lineHeight, 12, mLayout.lineHeight, mTheme.terminalCursor);
}

void Renderer::renderHighlightedRange(const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY)
{

    std::string selectedText = mTextLayout.expandTabs(text.substr(col, length));
    int x = mLayout.marginLeft + mTextLayout.columnToPixel(text, col) - mScrollOffsetX;
    int y = screenY(row, scrollOffsetY);
    int w = mTextLayout.width(selectedText);
    int h = mLayout.lineHeight;
    drawRect(x, y, w, h, mTheme.selection);
}

void Renderer::renderSearchOverlay(const SearchSession &session)
{
    uint32_t currMatch = session.hasMatches() ? session.getCurrentMatchIndex() + 1 : 0;
    const std::string &matchStr = std::to_string(currMatch) + "/" + std::to_string(session.getMatches().size());
    mSearchLayout.matchBoxWidth = mTextLayout.width(matchStr) + mSearchLayout.matchBoxPadding;

    drawRect(mSearchLayout.queryX, mSearchLayout.queryY, mSearchLayout.queryWidth, mSearchLayout.queryHeight, mTheme.overlayBackground);
    drawRect(mSearchLayout.matchBoxX, mSearchLayout.queryY, mSearchLayout.matchBoxWidth, mSearchLayout.queryHeight, mTheme.overlayBackground);

    SDL_Rect searchClipRect{
        static_cast<int>(mSearchLayout.queryX + mSearchLayout.textPadding),
        static_cast<int>(mSearchLayout.queryY),
        static_cast<int>(mSearchLayout.queryWidth - (mSearchLayout.textPadding * 2)),
        static_cast<int>(mSearchLayout.queryHeight)};
    CSF(SDL_SetRenderClipRect(mRenderer, &searchClipRect));

    const std::string &query = session.getQuery();
    drawText(query, mSearchLayout.textX - mScrollOffsetXSearch, mSearchLayout.textY);
    CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
    drawText(matchStr, mSearchLayout.matchBoxTextX, mSearchLayout.textY);
}

void Renderer::renderSearchCursor(const SearchSession &session)
{
    if (mCursorBlinker.visible())
    {
        uint32_t cursorTextWidth = mTextLayout.width(session.getQuery().substr(0, session.getCursor()));
        int cursorX = mSearchLayout.queryX + mSearchLayout.textPadding + cursorTextWidth - mScrollOffsetXSearch;
        SDL_Rect searchClipRect{
            static_cast<int>(mSearchLayout.queryX + mSearchLayout.textPadding),
            static_cast<int>(mSearchLayout.queryY),
            static_cast<int>(mSearchLayout.queryWidth - (mSearchLayout.textPadding * 2)),
            static_cast<int>(mSearchLayout.queryHeight)};
        CSF(SDL_SetRenderClipRect(mRenderer, &searchClipRect));
        drawRect(cursorX, mSearchLayout.textY, 2, mLayout.lineHeight, mTheme.cursor);
        CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
    }
}

void Renderer::renderSearchMatches(const SearchSession &session, const Editor &editor)
{
    if (session.getMatches().size() == 0)
    {
        return;
    }
    for (SearchMatch &match : session.getMatches())
    {
        const std::string &line = editor.getLineString(match.row);
        renderHighlightedRange(line, match.row, match.col, match.length, editor.getScrollOffsetY());
    }
}

void Renderer::updateEditor(Editor &editor)
{
    if (editor.consumeActivity())
    {
        mCursorBlinker.reset();
    }
    if (editor.isTerminalVisible())
    {
        mLayout.windowHeight = mLayout.totalWindowHeight * 0.75;
        editor.getTerminal().setVisibleRows(((mTerminalLayout.windowHeight - mTerminalLayout.marginTop) / mLayout.lineHeight) - 1);
        editor.adjustCursor((mLayout.windowHeight - mLayout.marginTop) / mLayout.lineHeight);
    }
    else
    {
        mLayout.windowHeight = mLayout.totalWindowHeight;
    }
    mCursorBlinker.update();
    clear();
    renderEditor(editor);
    editor.setVisibleRows(((mLayout.windowHeight - mLayout.marginTop) / mLayout.lineHeight));
    Cursor cursor = editor.getCursor();
    ensureCursorVisibleHorizontally(cursor, editor.getLineString(cursor.row));
    present();
}

void Renderer::updateFileBrowser(FileBrowser &browser)
{
    clear();
    renderFileBrowserSelection(browser);

    std::string currentPathStr = browser.getCurrentDir().string();
    drawText(currentPathStr, mLayout.marginLeft, mLayout.marginTop);

    uint32_t fileListTopMargin = mLayout.marginTop + (mLayout.lineHeight * 2);

    uint32_t visibleFiles = (mLayout.windowHeight - fileListTopMargin) / mLayout.lineHeight;
    browser.setVisibleFiles(visibleFiles);
    SDL_Color color;
    std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    uint32_t first = browser.getScrollOffset();
    uint32_t last = std::min(static_cast<int>(first + visibleFiles), static_cast<int>(filesToRender.size()));
    for (size_t i = first; i < last; ++i)
    {
        std::string file = filesToRender[i];
        color = mTheme.text;
        if (std::filesystem::is_directory(currentPathStr / std::filesystem::path{file}))
        {
            color = mTheme.fileBrowserDir;
        }
        std::string extension = browser.getFileExtension(file);
        uint32_t first = browser.getScrollOffset();
        file = fitTextToWidthFile(file, extension);
        drawText(file, mLayout.marginLeft, screenYBrowser(i, first, fileListTopMargin), color);
    }
    present();
}

void Renderer::renderFileBrowserSelection(FileBrowser &browser)
{
    std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    int x = mLayout.marginLeft;
    int y = screenYBrowser(browser.getSelectedIndex(), browser.getScrollOffset(), mLayout.marginTop + (mLayout.lineHeight * 2));
    int w = mTextLayout.width(filesToRender[browser.getSelectedIndex()]);
    int h = mLayout.lineHeight;
    drawRect(x, y, w, h, mTheme.selection);
}

/*
    Could probably be optimized, but will change when renderer changes anyways, so good luck future me :)
*/
const std::string Renderer::fitTextToWidthFile(const std::string &text, std::string &extension)
{
    uint32_t visibleWidth = mLayout.windowWidth - mLayout.marginLeft - mTextLayout.width("...") - mTextLayout.width(extension);

    if (mTextLayout.width(text) - mTextLayout.width(extension) <= visibleWidth)
    {
        return text;
    }

    uint32_t low = 0;
    uint32_t high = text.length() - extension.size();
    uint32_t bestLength = 0;

    while (low <= high)
    {
        uint32_t mid = low + (high - low) / 2;

        const std::string subs = text.substr(0, mid);

        if (mTextLayout.width(subs) <= visibleWidth)
        {
            bestLength = mid;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return (text.substr(0, bestLength) + "..." + extension);
}

void Renderer::present()
{
    CSF(SDL_RenderPresent(mRenderer));
}

void Renderer::onResize(uint32_t w, uint32_t h)
{
    mLayout.windowWidth = w;
    mLayout.windowHeight = h;
}

void Renderer::setFontSize()
{
    TTF_SetFontSize(mFont, mFontSize);
    mLayout.lineHeight = getLineHeight();
}

void Renderer::handlePlus(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld)
    {
        mFontSize++;
        setFontSize();
    }
}

void Renderer::handleMinus(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld)
    {
        mFontSize--;
        setFontSize();
    }
}

void Renderer::ensureCursorVisibleHorizontally(const Cursor &cursor, const std::string &line)
{
    int cursorPixelX = mTextLayout.columnToPixel(line, cursor.col);
    mEditorScrollPort.visibleWidth = mLayout.windowWidth - mLayout.marginLeft - mLayout.lineNumberAreaWidth;
    mEditorScrollPort.ensureVisible(cursorPixelX, 20);
    mScrollOffsetX = mEditorScrollPort.offsetX;
}

void Renderer::ensureCursorVisibleHorizontallySearch(uint32_t cursor, const std::string &line)
{
    int cursorPixelX = mTextLayout.columnToPixel(line, cursor);

    mSearchScrollPort.visibleWidth = mSearchLayout.queryWidth - mSearchLayout.textPadding * 2;
    mSearchScrollPort.ensureVisible(cursorPixelX, 2);
    mScrollOffsetXSearch = mSearchScrollPort.offsetX;
}

int Renderer::screenY(uint32_t row, uint32_t scrollOffset) const
{
    return mLayout.marginTop + (row - scrollOffset) * mLayout.lineHeight;
}

int Renderer::screenYBrowser(uint32_t row, uint32_t scrollOffset, uint32_t margin) const
{
    return margin + (row - scrollOffset) * mLayout.lineHeight;
}

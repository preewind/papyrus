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
    mFont = TTF_OpenFont("assets/JetBrainsMono-Regular.ttf", 20);

    if (!mFont)
    {
        throw std::runtime_error("Failed to load font");
    }
    // editor layout
    mLayout.lineHeight = getLineHeight();
    SDL_GetWindowSize(window, (int *)&mLayout.windowWidth, (int *)&mLayout.totalWindowHeight);
    mLayout.windowHeight = mLayout.totalWindowHeight;

    mTerminalLayout.windowHeight = 0.2 * mLayout.totalWindowHeight;
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

uint32_t Renderer::measureTextWidth(const std::string &text)
{
    if (text.empty())
    {
        return 0;
    }
    int w = 0;
    int h = 0;

    CSF(TTF_GetStringSize(mFont, text.c_str(), 0, &w, &h));

    return w;
}

int Renderer::getLineHeight() const
{
    return TTF_GetFontHeight(mFont);
}

const EditorLayout &Renderer::getEditorLayout() const
{
    return mLayout;
}

std::string Renderer::expandTabs(const std::string &text)
{
    std::string result = "";

    for (char c : text)
    {
        if (c == '\t')
        {
            result += "    ";
        }
        else
        {
            result += c;
        }
    }
    return result;
}

SDL_Color Renderer::getColorFromTokenType(const Token &token)
{
    switch (token.type)
    {
    case TokenType::OpenCurly:
    case TokenType::CloseCurly:
    case TokenType::OpenParen:
    case TokenType::CloseParen:
        return {255, 255, 0, 255};
        break;
    case TokenType::Comment:
        return hexToSDLColor("#8B949E");
        break;
    case TokenType::String:
        return hexToSDLColor("#A5D6FF");
        break;
    case TokenType::Keyword:
        return hexToSDLColor("#FF7B72");
        break;
    case TokenType::Preprocessor:
        return hexToSDLColor("#C586C0");
        break;
    case TokenType::IncludeLib:
        return hexToSDLColor("#A5D6FF");
        break;
    default:
        return {255, 255, 255, 255};
    }
}

void Renderer::drawText(const std::string &text, int x, int y, SDL_Color color = {255, 255, 255, 255})
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
    std::string expandedLine = expandTabs(text);
    for (const Token &token : tokens)
    {
        uint32_t vCol = getVirtualCol(text, token.col);
        const std::string &subs = expandedLine.substr(vCol, token.length);
        int xOffset = measureTextWidth(std::string(vCol, ' '));
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

void Renderer::resetCursorBlink()
{
    mCursorVisible = true;
    mLastBlink = SDL_GetTicks();
}

void Renderer::renderLineNumbers(uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows)
{
    uint32_t first = scrollOffsetY;
    uint32_t last = std::min(first + visibleRows, numLines);
    for (uint32_t i = first; i < last; ++i)
    {

        drawText(std::to_string(i + 1), mLayout.lineNumberAreaWidth / 2 - measureTextWidth(std::to_string(i + 1)) / 2, screenY(i, scrollOffsetY), SDL_Color{66, 67, 68, 255});
    }
}

void Renderer::renderCursor(const Cursor &cursor, const std::string &text, uint32_t offsetY)
{
    if (mCursorVisible)
    {
        int x = textX(text, cursor.col);
        int y = screenY(cursor.row, offsetY);

        drawRect(x, y, 2, mLayout.lineHeight, SDL_Color{255, 255, 255, 255});
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
            drawText(expandTabs(text[i]), mLayout.marginLeft - mScrollOffsetX, screenY(i, first));
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
        renderSearchOverlay(editor.getSearch());
        renderSearchCursor(editor.getSearch());
    }
    CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
    if (editor.isTerminalVisible())
    {
        renderTerminal(editor);
        renderTerminalCursor(editor.getTerminal());
    }
}

void Renderer::renderTerminal(const Editor &editor)
{
    drawRect(mTerminalLayout.windowX, mTerminalLayout.windowY, mLayout.windowWidth, mTerminalLayout.windowHeight, SDL_Color{31, 32, 33, 255});
    Terminal terminal = editor.getTerminal();
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    drawText(text, mTerminalLayout.windowX + mTerminalLayout.marginLeft, mLayout.totalWindowHeight - mTerminalLayout.marginTop - mLayout.lineHeight);
}

void Renderer::renderTerminalCursor(const Terminal &terminal)
{
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    uint32_t cursorTextWidth = measureTextWidth(text.substr(0,text.size()+ terminal.getCursor()-terminal.getInput().size()));
    drawRect(mTerminalLayout.windowX + mTerminalLayout.marginLeft + cursorTextWidth, mLayout.totalWindowHeight - mTerminalLayout.marginTop - mLayout.lineHeight, 12, mLayout.lineHeight, SDL_Color{255, 255, 255, 255});
}

void Renderer::renderHighlightedRange(const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY, SDL_Color color)
{

    std::string selectedText = expandTabs(text.substr(col, length));
    int x = textX(text, col);
    int y = screenY(row, scrollOffsetY);
    int w = measureTextWidth(selectedText);
    int h = mLayout.lineHeight;
    drawRect(x, y, w, h, color);
}

void Renderer::renderSearchOverlay(const SearchSession &session)
{
    uint32_t currMatch = session.hasMatches() ? session.getCurrentMatchIndex() + 1 : 0;
    const std::string &matchStr = std::to_string(currMatch) + "/" + std::to_string(session.getMatches().size());
    mSearchLayout.matchBoxWidth = measureTextWidth(matchStr) + mSearchLayout.matchBoxPadding;

    drawRect(mSearchLayout.queryX, mSearchLayout.queryY, mSearchLayout.queryWidth, mSearchLayout.queryHeight, mSearchLayout.rectColor);
    drawRect(mSearchLayout.matchBoxX, mSearchLayout.queryY, mSearchLayout.matchBoxWidth, mSearchLayout.queryHeight, mSearchLayout.rectColor);
    drawText(session.getQuery(), mSearchLayout.textX, mSearchLayout.textY);

    drawText(matchStr, mSearchLayout.matchBoxTextX, mSearchLayout.textY);
}

void Renderer::renderSearchCursor(const SearchSession &session)
{
    if (mCursorVisible)
    {
        uint32_t cursorTextWidth = measureTextWidth(session.getQuery().substr(0, session.getCursor()));
        drawRect(mSearchLayout.queryX + mSearchLayout.textPadding + cursorTextWidth, mSearchLayout.textY, 2, mLayout.lineHeight, SDL_Color{255, 255, 255, 255});
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
        renderHighlightedRange(line, match.row, match.col, match.length, editor.getScrollOffsetY(), SDL_Color{46, 47, 108, 255});
    }
}

void Renderer::updateCursor()
{
    Uint64 now = SDL_GetTicks();
    if (now - mLastBlink > 500)
    {
        mCursorVisible = !mCursorVisible;
        mLastBlink = now;
    }
}

void Renderer::updateEditor(Editor &editor)
{
    if (editor.consumeActivity())
    {
        resetCursorBlink();
    }
    if (editor.isTerminalVisible())
    {
        mLayout.windowHeight = mLayout.totalWindowHeight * 0.8;
    }
    else
    {
        mLayout.windowHeight = mLayout.totalWindowHeight;
    }
    updateCursor();
    clear();
    renderEditor(editor);
    editor.setVisibleRows((mLayout.windowHeight - mLayout.marginTop) / mLayout.lineHeight);
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
        color = {255, 255, 255, 255};
        if (std::filesystem::is_directory(currentPathStr / std::filesystem::path{file}))
        {
            color = {255, 255, 0, 255};
        }
        std::string extension = browser.getFileExtension(file);
        uint32_t first = browser.getScrollOffset();
        file = fitTextToWidth(file, extension);
        drawText(file, mLayout.marginLeft, screenYBrowser(i, first, fileListTopMargin), color);
    }
    present();
}

void Renderer::renderFileBrowserSelection(FileBrowser &browser)
{
    std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    int x = mLayout.marginLeft;
    int y = screenYBrowser(browser.getSelectedIndex(), browser.getScrollOffset(), mLayout.marginTop + (mLayout.lineHeight * 2));
    int w = measureTextWidth(filesToRender[browser.getSelectedIndex()]);
    int h = mLayout.lineHeight;
    drawRect(x, y, w, h, SDL_Color{46, 47, 108, 255});
}

/*
    Could probably be optimized, but will change when renderer changes anyways, so good luck future me :)
*/
const std::string Renderer::fitTextToWidth(const std::string &text, std::string &extension)
{
    uint32_t visibleWidth = mLayout.windowWidth - mLayout.marginLeft - measureTextWidth("...") - measureTextWidth(extension);

    if (measureTextWidth(text) - measureTextWidth(extension) <= visibleWidth)
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

        if (measureTextWidth(subs) <= visibleWidth)
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

uint32_t Renderer::getVirtualCol(const std::string &text, uint32_t rawCol)
{
    uint32_t virtualCol = 0;
    for (uint32_t i = 0; i < rawCol && i < text.size(); ++i)
    {
        if (text[i] == '\t')
        {
            virtualCol += 4;
        }
        else
        {
            virtualCol += 1;
        }
    }
    return virtualCol;
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

void Renderer::ensureCursorVisibleHorizontally(const Cursor &cursor, const std::string &line)
{
    int cursorPixelX = measureTextWidth(expandTabs(line.substr(0, cursor.col)));

    int visibleWidth =
        mLayout.windowWidth - mLayout.marginLeft - mLayout.lineNumberAreaWidth;

    if (cursorPixelX < mScrollOffsetX)
    {
        mScrollOffsetX = cursorPixelX;
    }
    else if (cursorPixelX > mScrollOffsetX + visibleWidth)
    {
        mScrollOffsetX = cursorPixelX - visibleWidth + 20;
    }
}

int Renderer::textX(const std::string &line, uint32_t col)
{
    return mLayout.marginLeft + measureTextWidth(expandTabs(line.substr(0, col))) - mScrollOffsetX;
}

int Renderer::screenY(uint32_t row, uint32_t scrollOffset) const
{
    return mLayout.marginTop + (row - scrollOffset) * mLayout.lineHeight;
}

int Renderer::screenYBrowser(uint32_t row, uint32_t scrollOffset, uint32_t margin) const
{
    return margin + (row - scrollOffset) * mLayout.lineHeight;
}

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
    SDL_GetWindowSize(window, (int *)&mLayout.totalWindowWidth, (int *)&mLayout.totalWindowHeight);
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

void Renderer::setLayoutManager(const LayoutManager &layoutManager)
{
    mLayoutManager = layoutManager;
}

int Renderer::getLineHeight() const
{
    return TTF_GetFontHeight(mFont);
}

const LayoutConfig &Renderer::getLayoutConfig() const
{
    return mLayoutManager.getLayoutConfig();
}

const SDL_Properties &Renderer::getSDL_Properties() const
{
    return mLayout;
}

const Theme &Renderer::getTheme() const
{
    return mTheme;
}

TTF_Font *Renderer::getFont() const
{
    return mFont;
}

const CursorBlinker &Renderer::getCursorBlinker() const
{
    return mCursorBlinker;
}

uint32_t Renderer::getScrollOffsetXSearch() const
{
    return mScrollOffsetXSearch;
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

void Renderer::drawTextTokenized(const std::string &text, uint32_t y, const std::vector<Token> &tokens, uint32_t scrollOffsetX)
{
    std::string expandedLine = mTextLayout.expandTabs(text);
    for (const Token &token : tokens)
    {
        uint32_t vCol = mTextLayout.virtualColumn(text, token.col);
        const std::string &subs = expandedLine.substr(vCol, token.length);
        int xOffset = mTextLayout.width(std::string(vCol, ' '));
        int renderX = getLayoutConfig().editorMarginLeft - scrollOffsetX + xOffset;
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

void Renderer::drawRect(Rect rect, SDL_Color color)
{
    drawRect(rect.x, rect.y, rect.w, rect.h, color);
}

void Renderer::pushClipRect(const SDL_Rect &rect)
{
    CSF(SDL_SetRenderClipRect(mRenderer, &rect));
}

void Renderer::clearClipRect()
{
    CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
}

void Renderer::renderHighlightedRange(const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY, uint32_t scrollOffsetX)
{

    std::string selectedText = mTextLayout.expandTabs(text.substr(col, length));
    int x = getLayoutConfig().editorMarginLeft + mTextLayout.columnToPixel(text, col) - scrollOffsetX;
    int y = screenY(row, scrollOffsetY);
    int w = mTextLayout.width(selectedText);
    int h = mLayout.lineHeight;
    drawRect(x, y, w, h, mTheme.selection);
}

void Renderer::updateEditor(Editor &editor)
{
    if (editor.consumeActivity())
    {
        mCursorBlinker.reset();
    }
    
    mCursorBlinker.update();
}

void Renderer::updateFileBrowser(FileBrowser &browser)
{
    clear();
    mFileBrowserView.render(*this, browser, mTextLayout, mLayoutManager.getLayoutConfig(), getSDL_Properties());
    present();
}

/*
    Could probably be optimized, but will change when renderer changes anyways, so good luck future me :)
*/
const std::string Renderer::fitTextToWidthFile(const std::string &text, std::string &extension)
{
    uint32_t visibleWidth = mLayout.totalWindowWidth - getLayoutConfig().editorMarginLeft - mTextLayout.width("...") - mTextLayout.width(extension);

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
    mLayout.totalWindowWidth = w;
    mLayout.totalWindowHeight = h;
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

/*
void Renderer::ensureCursorVisibleHorizontallySearch(uint32_t cursor, const std::string &line)
{
    int cursorPixelX = mTextLayout.columnToPixel(line, cursor);
    mSearchScrollPort.visibleWidth = getSearchLayout().queryBox.w - getSearchLayout().textPadding * 2;
    mSearchScrollPort.ensureVisible(cursorPixelX, 2);
    mScrollOffsetXSearch = mSearchScrollPort.offsetX;
}
*/

int Renderer::screenY(uint32_t row, uint32_t scrollOffset) const
{
    return getLayoutConfig().editorMarginTop + (row - scrollOffset) * mLayout.lineHeight;
}

int Renderer::screenYBrowser(uint32_t row, uint32_t scrollOffset, uint32_t margin) const
{
    return margin + (row - scrollOffset) * mLayout.lineHeight;
}

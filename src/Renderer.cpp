#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>

#include "Renderer.h"
#include "IRenderBackend.h"
#include "ITextMeasurer.h"
#include "SDLRenderBackend.h"
#include "Editor.h"
#include "FileBrowser.h"
#include "SearchSession.h"
#include "logger.h"
#include "util.h"

Renderer::Renderer(SDL_Window *window)
{
    auto backend = std::make_unique<SDLRenderBackend>(window, "assets/JetBrainsMono-Regular.ttf", mFontSize);
    mBackend = std::move(backend);

    mTextLayout.setMeasurer(mBackend.get());
    mLayout.lineHeight = getLineHeight();
    SDL_GetWindowSize(window, (int *)&mLayout.totalWindowWidth, (int *)&mLayout.totalWindowHeight);
}

Renderer::~Renderer()
{
}

void Renderer::clear()
{
    mBackend->clear(RenderColor{0, 0, 0, 255});
}

int Renderer::getLineHeight() const
{
    return mBackend->lineHeight();
}

const SDL_Properties &Renderer::getSDL_Properties() const
{
    return mLayout;
}

const Theme &Renderer::getTheme() const
{
    return mTheme;
}

const ITextMeasurer &Renderer::getTextMeasurer() const
{
    return *mBackend;
}

const CursorBlinker &Renderer::getCursorBlinker() const
{
    return mCursorBlinker;
}

RenderColor Renderer::getColorFromTokenType(const Token &token)
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

void Renderer::drawText(const std::string &text, int x, int y, RenderColor color)
{
    mBackend->drawText(text, x, y, color);
}

void Renderer::drawTextTokenized(const std::string &text, uint32_t y, const std::vector<Token> &tokens, uint32_t scrollOffsetX, const LayoutConfig &layoutConfig)
{
    std::string expandedLine = mTextLayout.expandTabs(text);
    for (const Token &token : tokens)
    {
        uint32_t vCol = mTextLayout.virtualColumn(text, token.col);
        const std::string &subs = expandedLine.substr(vCol, token.length);
        int xOffset = mTextLayout.width(std::string(vCol, ' '));
        int renderX = layoutConfig.editorMarginLeft - scrollOffsetX + xOffset;
        drawText(subs, renderX, y, getColorFromTokenType(token));
    }
}

void Renderer::drawRect(int x, int y, int w, int h, RenderColor color)
{
    mBackend->fillRect(RenderRect{x, y, w, h}, color);
}

void Renderer::drawRect(Rect rect, RenderColor color)
{
    drawRect(rect.x, rect.y, rect.w, rect.h, color);
}

void Renderer::pushClipRect(const Rect &rect)
{
    mBackend->setClipRect(RenderRect{static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.w), static_cast<int>(rect.h)});
}

void Renderer::clearClipRect()
{
    mBackend->clearClipRect();
}

void Renderer::renderHighlightedRange(const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY, uint32_t scrollOffsetX, const LayoutConfig &layoutConfig)
{

    std::string selectedText = mTextLayout.expandTabs(text.substr(col, length));
    int x = layoutConfig.editorMarginLeft + mTextLayout.columnToPixel(text, col) - scrollOffsetX;
    int y = screenY(row, scrollOffsetY, layoutConfig.editorMarginTop);
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

void Renderer::updateFileBrowser(FileBrowser &browser, const LayoutConfig &layoutConfig)
{
    clear();
    mFileBrowserView.render(*this, browser, mTextLayout, layoutConfig, getSDL_Properties());
    present();
}

/*
    Could probably be optimized, but will change when renderer changes anyways, so good luck future me :)
*/
const std::string Renderer::fitTextToWidthFile(const std::string &text, std::string &extension, const LayoutConfig &layoutConfig)
{
    uint32_t visibleWidth = mLayout.totalWindowWidth - layoutConfig.editorMarginLeft - mTextLayout.width("...") - mTextLayout.width(extension);

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
    mBackend->present();
}

void Renderer::onResize(uint32_t w, uint32_t h)
{
    mLayout.totalWindowWidth = w;
    mLayout.totalWindowHeight = h;
}

void Renderer::setFontSize()
{
    mBackend->setFontSize(mFontSize);
    mLayout.lineHeight = getLineHeight();
}

void Renderer::handlePlus()
{
    mFontSize++;
    setFontSize();
}

void Renderer::handleMinus()
{
    mFontSize--;
    setFontSize();
}

int Renderer::screenY(uint32_t row, uint32_t scrollOffset, uint32_t editorMarginTop) const
{
    return editorMarginTop + (row - scrollOffset) * mLayout.lineHeight;
}

int Renderer::screenYBrowser(uint32_t row, uint32_t scrollOffset, uint32_t margin) const
{
    return margin + (row - scrollOffset) * mLayout.lineHeight;
}

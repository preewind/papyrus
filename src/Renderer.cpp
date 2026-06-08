#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>

#include "Renderer.h"
#include "Editor.h"
#include "logger.h"
#include "util.h"

Renderer::Renderer(SDL_Window *window)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);

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

    mLayout.lineHeight = getLineHeight();
    SDL_GetWindowSize(window, (int *)&mLayout.windowWidth, (int *)&mLayout.windowHeight);
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

int Renderer::measureTextWidth(const std::string &text)
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

    for (size_t i = first; i < last; ++i)
    {
        drawText(expandTabs(text[i]), mLayout.marginLeft - mScrollOffsetX, screenY(i, first));
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
        std::string selectedText = expandTabs(line.substr(beginCol, endCol - beginCol));
        int x = textX(line, beginCol);
        int y = screenY(row, editor.getScrollOffsetY());
        int w = measureTextWidth(selectedText);
        int h = mLayout.lineHeight;
        LOG_DEBUG() << selectedText;
        drawRect(x, y, w, h, SDL_Color{46, 47, 48, 255});
    }
}

void Renderer::renderEditor(const Editor &editor)
{
    renderLineNumbers(editor.getLineCount(), editor.getScrollOffsetY(), editor.getVisibleRows());
    // ensure 
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

    Cursor cursor = editor.getCursor();
    std::string currentLineText = editor.getLineString(cursor.row);
    renderCursor(cursor, currentLineText, editor.getScrollOffsetY());
    renderText(editor);
    SDL_SetRenderClipRect(mRenderer, nullptr);
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

void Renderer::update(Editor &editor)
{
    if (editor.consumeActivity())
    {
        resetCursorBlink();
    }
    updateCursor();
    clear();
    renderEditor(editor);
    editor.setVisibleRows((mLayout.windowHeight - mLayout.marginTop) / mLayout.lineHeight);
    Cursor cursor = editor.getCursor();
    ensureCursorVisibleHorizontally(cursor, editor.getLineString(cursor.row));
    present();
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

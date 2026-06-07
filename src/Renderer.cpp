#include <stdexcept>
#include <iostream>

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

void Renderer::drawText(const std::string &text, int x, int y)
{
    if (text.empty())
        return;

    SDL_Color color = {255, 255, 255, 255};

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

void Renderer::renderCursor(const Cursor &cursor, const std::string &text)
{
    if (mCursorVisible)
    {
        int x = 20 + measureTextWidth(expandTabs(text.substr(0, cursor.col)));
        int y = 20 + cursor.row * getLineHeight();

        drawRect(x, y, 2, getLineHeight(), SDL_Color{255, 255, 255, 255});
    }
}

void Renderer::renderText(const std::vector<std::string> &text)
{
    for (size_t i = 0; i < text.size(); ++i)
    {
        drawText(expandTabs(text[i]), 20, 20 + getLineHeight() * i);
    }
}

void Renderer::renderSelection(const Editor &editor)
{
    Selection selection = editor.getSelection().normalized();

    if(selection.empty()) return;

    const Position& start = selection.begin;
    const Position& end = selection.end;

    for(size_t row=start.row; row <= end.row; ++row){
        int beginCol, endCol;
        if(row == start.row){
            
            beginCol = start.col;
            // if only one line selected
            if(start.row == end.row){
                endCol = end.col;
            }
            else{
                endCol = editor.getLineString(row).size();
            }
        }
        // in between line -> should be fully selected
        else if(row < end.row){
            beginCol = 0;
            endCol = editor.getLineString(row).size();
        }
        else{
            beginCol = 0;
            endCol = end.col;
        }
        const std::string &line = editor.getLineString(row);
        std::string selectedText = expandTabs(line.substr(beginCol, endCol-beginCol));
        int x = 20 + measureTextWidth(expandTabs(line.substr(0, beginCol)));
        int y = 20 + row * getLineHeight();
        int w = measureTextWidth(selectedText);
        int h = getLineHeight();
        LOG_DEBUG() << selectedText;
        drawRect(x, y, w, h, SDL_Color{46, 47, 48, 255});
    }
}

void Renderer::renderEditor(const Editor &editor)
{
    if (editor.getSelectionActive())
    {
        renderSelection(editor);
    }

    Cursor cursor = editor.getCursor();
    std::string currentLineText = editor.getLineString(cursor.row);
    renderCursor(cursor, currentLineText);
    renderText(editor.getText());
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
    present();
}

void Renderer::present()
{
    CSF(SDL_RenderPresent(mRenderer));
}

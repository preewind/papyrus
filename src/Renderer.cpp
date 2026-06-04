#include <stdexcept>

#include "Renderer.h"
#include "util.h"

Renderer::Renderer(SDL_Window *window)
{
    renderer = SDL_CreateRenderer(window, nullptr);

    if(!renderer)
    {
        throw std::runtime_error("Failed to create renderer");
    }
    if (!TTF_Init())
    {
        throw std::runtime_error("Failed to initialize SDL_ttf");
    }
    font = TTF_OpenFont("assets/JetBrainsMono-Regular.ttf", 20);

    if (!font)
    {
        throw std::runtime_error("Failed to load font");
    }
}

Renderer::~Renderer()
{
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
}

void Renderer::clear()
{
    CSF(SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255));
    CSF(SDL_RenderClear(renderer));
}

int Renderer::measureTextWidth(const std::string &text)
{
    if(text.empty())
    {
        return 0;
    }
    int w = 0;
    int h = 0;

    CSF(TTF_GetStringSize(font, text.c_str(), 0, &w, &h));

    return w;
}

int Renderer::getLineHeight() const
{
    return TTF_GetFontHeight(font);
}

void Renderer::drawText(const std::string &text, int x, int y)
{
    if(text.empty())return;

    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface* surface =
        TTF_RenderText_Blended(
            font,
            text.c_str(),
            text.size(),
            color);

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface);

    SDL_FRect dst;
    dst.x = static_cast<float>(x);
    dst.y = static_cast<float>(y);
    dst.w = static_cast<float>(surface->w);
    dst.h = static_cast<float>(surface->h);

    CSF(SDL_RenderTexture(renderer, texture, nullptr, &dst));

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

    CSF(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a));

    CSF(SDL_RenderFillRect(renderer, &rect));
}

void Renderer::present()
{
    CSF(SDL_RenderPresent(renderer));
}

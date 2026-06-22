#include "SDLRenderBackend.h"

#include "util.h"

SDLRenderBackend::SDLRenderBackend(SDL_Window *window, const std::string &fontPath, uint8_t fontSize)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);
    CSP(mRenderer);

    setVSync(true);

    CSF(TTF_Init());

    mFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    CSP(mFont);
}

SDLRenderBackend::~SDLRenderBackend()
{
    if (mFont)
    {
        TTF_CloseFont(mFont);
    }
    TTF_Quit();

    if (mRenderer)
    {
        SDL_DestroyRenderer(mRenderer);
    }
}

void SDLRenderBackend::setVSync(bool enabled)
{
    CSF(SDL_SetRenderVSync(mRenderer, enabled ? 1 : 0));
}

void SDLRenderBackend::clear(const RenderColor &color)
{
    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));
    CSF(SDL_RenderClear(mRenderer));
}

void SDLRenderBackend::present()
{
    CSF(SDL_RenderPresent(mRenderer));
}

void SDLRenderBackend::fillRect(const RenderRect &rect, const RenderColor &color)
{
    SDL_FRect sdlRect;
    sdlRect.x = static_cast<float>(rect.x);
    sdlRect.y = static_cast<float>(rect.y);
    sdlRect.w = static_cast<float>(rect.w);
    sdlRect.h = static_cast<float>(rect.h);

    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));
    CSF(SDL_RenderFillRect(mRenderer, &sdlRect));
}

void SDLRenderBackend::setClipRect(const RenderRect &rect)
{
    SDL_Rect sdlRect;
    sdlRect.x = rect.x;
    sdlRect.y = rect.y;
    sdlRect.w = rect.w;
    sdlRect.h = rect.h;

    CSF(SDL_SetRenderClipRect(mRenderer, &sdlRect));
}

void SDLRenderBackend::clearClipRect()
{
    CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
}

void SDLRenderBackend::drawText(const std::string &text, int x, int y, const RenderColor &color)
{
    if (text.empty())
    {
        return;
    }

    SDL_Color sdlColor;
    sdlColor.r = color.r;
    sdlColor.g = color.g;
    sdlColor.b = color.b;
    sdlColor.a = color.a;

    SDL_Surface *surface = TTF_RenderText_Blended(mFont, text.c_str(), text.size(), sdlColor);
    CSP(surface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    CSP(texture);

    SDL_FRect dst;
    dst.x = static_cast<float>(x);
    dst.y = static_cast<float>(y);
    dst.w = static_cast<float>(surface->w);
    dst.h = static_cast<float>(surface->h);

    CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

uint32_t SDLRenderBackend::width(std::string_view text) const
{
    if (text.empty())
    {
        return 0;
    }

    int w = 0;
    int h = 0;
    CSF(TTF_GetStringSize(mFont, text.data(), text.size(), &w, &h));
    return static_cast<uint32_t>(w);
}

int SDLRenderBackend::lineHeight() const
{
    return TTF_GetFontHeight(mFont);
}

void SDLRenderBackend::setFontSize(uint8_t size)
{
    CSF(TTF_SetFontSize(mFont, size));
}

SDL_Renderer *SDLRenderBackend::nativeRenderer() const
{
    return mRenderer;
}

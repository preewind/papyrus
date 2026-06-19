#include <memory>

#include "Renderer.h"
#include "IRenderBackend.h"
#include "ITextMeasurer.h"
#include "SDLRenderBackend.h"
#include "Editor.h"

Renderer::Renderer(SDL_Window *window, uint8_t fontSize)
{
    auto backend = std::make_unique<SDLRenderBackend>(window, "assets/JetBrainsMono-Regular.ttf", fontSize);
    mBackend = std::move(backend);

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

void Renderer::drawText(const std::string &text, int x, int y)
{
    drawText(text, x, y, mTheme.text);
}

void Renderer::drawText(const std::string &text, int x, int y, RenderColor color)
{
    mBackend->drawText(text, x, y, color);
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

void Renderer::present()
{
    mBackend->present();
}

void Renderer::onResize(uint32_t w, uint32_t h)
{
    mLayout.totalWindowWidth = w;
    mLayout.totalWindowHeight = h;
}

void Renderer::setFontSize(uint8_t fontSize)
{
    mBackend->setFontSize(fontSize);
    mLayout.lineHeight = getLineHeight();
}

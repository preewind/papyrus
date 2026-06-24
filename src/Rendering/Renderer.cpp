#include "Renderer.h"

Renderer::Renderer(IRenderBackend &backend, const Theme &theme, uint32_t windowWidth, uint32_t windowHeight)
{
    mBackend = &backend;
    mTheme = &theme;

    mLayout.lineHeight = getLineHeight();
    mLayout.totalWindowWidth = windowWidth;
    mLayout.totalWindowHeight = windowHeight;
}

void Renderer::clear()
{
    mBackend->clear(mTheme->background);
}

void Renderer::clear(RenderColor color)
{
    mBackend->clear(color);
}

int Renderer::getLineHeight() const
{
    return mBackend->lineHeight();
}

const Window_Properties &Renderer::getWindowProperties() const
{
    return mLayout;
}

const Theme &Renderer::getTheme() const
{
    return *mTheme;
}

void Renderer::drawText(const std::string &text, int x, int y)
{
    drawText(text, x, y, mTheme->text);
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

void Renderer::loadTexture(float x, float y, float w, float h, const std::filesystem::path &file)
{
    mBackend->loadTexture(x, y, w, h, file);
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

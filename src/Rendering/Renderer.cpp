#include "Renderer.h"

#include <string>

#include "logger.h"

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

void Renderer::loadTextureByName(float x, float y, float w, float h, std::string_view assetName, float rotation)
{
    const std::filesystem::path *assetPath = findTextureAsset(assetName);
    if (assetPath == nullptr)
    {
        LOG_ERROR() << "Missing texture: " << assetName;
        return;
    }

    mBackend->loadTexture(x, y, w, h, *assetPath, rotation);
}

void Renderer::loadAnimationByName(float x, float y, float w, float h,
                                   std::string_view assetName,
                                   uint32_t elapsedMs,
                                   AnimationPlaybackMode playbackMode,
                                   float rotation)
{
    const std::filesystem::path *assetPath = findAnimationAsset(assetName);
    if (assetPath == nullptr)
    {
        LOG_ERROR() << "Missing animation: " << assetName;
        return;
    }

    mBackend->loadAnimation(x, y, w, h, *assetPath, elapsedMs, playbackMode, rotation);
}

void Renderer::registerTextureAsset(const std::string &assetName, const std::filesystem::path &file)
{
    mTextureAssets[assetName] = file;
}

void Renderer::registerAnimationAsset(const std::string &assetName, const std::filesystem::path &file)
{
    mAnimationAssets[assetName] = file;
}

bool Renderer::preloadTextureByName(std::string_view assetName)
{
    const std::filesystem::path *assetPath = findTextureAsset(assetName);
    if (assetPath == nullptr)
    {
        LOG_ERROR() << "Missing texture: " << assetName;
        return false;
    }

    return preloadTexture(*assetPath);
}

bool Renderer::preloadAnimationByName(std::string_view assetName)
{
    const std::filesystem::path *assetPath = findAnimationAsset(assetName);
    if (assetPath == nullptr)
    {
        LOG_ERROR() << "Missing animation: " << assetName;
        return false;
    }

    return preloadAnimation(*assetPath);
}

bool Renderer::preloadTexture(const std::filesystem::path &file)
{
    return mBackend->preloadTexture(file);
}

bool Renderer::preloadAnimation(const std::filesystem::path &file)
{
    return mBackend->preloadAnimation(file);
}

void Renderer::evictTexture(const std::filesystem::path &file)
{
    mBackend->evictTexture(file);
}

void Renderer::evictAnimation(const std::filesystem::path &file)
{
    mBackend->evictAnimation(file);
}

void Renderer::clearTextureCache()
{
    mBackend->clearTextureCache();
}

void Renderer::clearAnimationCache()
{
    mBackend->clearAnimationCache();
}

uint32_t Renderer::getAnimationDurationByName(std::string_view assetName) const
{
    const std::filesystem::path *assetPath = findAnimationAsset(assetName);
    if (assetPath == nullptr)
    {
        return 0;
    }

    return mBackend->getAnimationDurationMs(*assetPath);
}

std::pair<uint32_t, uint32_t> Renderer::getAnimationDimensionsByName(std::string_view assetName) const
{
    const std::filesystem::path *assetPath = findAnimationAsset(assetName);
    if (assetPath == nullptr)
    {
        return {0, 0};
    }

    return mBackend->getAnimationDimensions(*assetPath);
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

const std::filesystem::path *Renderer::findTextureAsset(std::string_view assetName) const
{
    auto entry = mTextureAssets.find(std::string(assetName));
    if (entry == mTextureAssets.end())
    {
        return nullptr;
    }

    return &entry->second;
}

const std::filesystem::path *Renderer::findAnimationAsset(std::string_view assetName) const
{
    auto entry = mAnimationAssets.find(std::string(assetName));
    if (entry == mAnimationAssets.end())
    {
        return nullptr;
    }

    return &entry->second;
}

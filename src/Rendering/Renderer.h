#pragma once

#include <cstdint>
#include <unordered_map>

#include "IRenderBackend.h"
#include "RenderContext.h"
#include "theme.h"
#include "types.h"

class Renderer : public RenderContext
{

public:
    Renderer(IRenderBackend &backend, const Theme &theme, uint32_t windowWidth, uint32_t windowHeight);
    ~Renderer() = default;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void clear();
    void clear(RenderColor color) override;
    int getLineHeight() const;
    const Window_Properties &getWindowProperties() const override;
    const Theme &getTheme() const override;

    void drawText(const std::string &text, int x, int y) override;
    void drawText(const std::string &text, int x, int y, RenderColor color) override;
    void drawRainbowText(const std::string &text, int x, int y) override;
    void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, RenderColor color) override;
    void drawRect(Rect rect, RenderColor color) override;
    void drawRect(float x, float y, float w, float h, RenderColor color) override;
    void drawRect(RectF rect, RenderColor color) override;
    void drawDottedLine(float x, float y, float length, float lineWidth, float spacing, float rectSize, const RenderColor &color);
    void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) override;
    void loadTextureByName(float x, float y, float w, float h, std::string_view assetName, float rotation = 0.0f) override;
    void loadAnimationByName(float x, float y, float w, float h, std::string_view assetName, uint32_t elapsedMs, AnimationPlaybackMode playbackMode, float rotation = 0.0f) override;
    uint32_t getAnimationDurationByName(std::string_view assetName) const override;
    std::pair<uint32_t, uint32_t> getAnimationDimensionsByName(std::string_view assetName) const override;
    void registerTextureAsset(const std::string &assetName, const std::filesystem::path &file);
    void registerAnimationAsset(const std::string &assetName, const std::filesystem::path &file);
    bool preloadTextureByName(std::string_view assetName);
    bool preloadAnimationByName(std::string_view assetName);
    bool preloadTexture(const std::filesystem::path &file);
    bool preloadAnimation(const std::filesystem::path &file);
    void evictTexture(const std::filesystem::path &file);
    void evictAnimation(const std::filesystem::path &file);
    void clearTextureCache();
    void clearAnimationCache();
    void pushClipRect(const Rect &rect) override;
    void clearClipRect() override;
    void present();

    void onResize(uint32_t w, uint32_t h);

    void setFontSize(uint8_t fontSize);

private:
    const std::filesystem::path *findTextureAsset(std::string_view assetName) const;
    const std::filesystem::path *findAnimationAsset(std::string_view assetName) const;

    IRenderBackend *mBackend = nullptr;
    const Theme *mTheme = nullptr;
    Window_Properties mLayout;
    std::unordered_map<std::string, std::filesystem::path> mTextureAssets;
    std::unordered_map<std::string, std::filesystem::path> mAnimationAssets;
};
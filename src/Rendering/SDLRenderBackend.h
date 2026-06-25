#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "IRenderBackend.h"

class SDLRenderBackend : public IRenderBackend
{
public:
    SDLRenderBackend(SDL_Window *window, const std::string &fontPath, uint8_t fontSize);
    ~SDLRenderBackend() override;

    SDLRenderBackend(const SDLRenderBackend &) = delete;
    SDLRenderBackend &operator=(const SDLRenderBackend &) = delete;

    void setVSync(bool enabled) override;
    void clear(const RenderColor &color) override;
    void present() override;

    void fillRect(const RenderRect &rect, const RenderColor &color) override;
    void setClipRect(const RenderRect &rect) override;
    void clearClipRect() override;

    void drawText(const std::string &text, int x, int y, const RenderColor &color) override;
    void rainBowText();
    void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) override;
    void loadAnimation(float x, float y, float w, float h,
                       const std::filesystem::path &file,
                       uint32_t elapsedMs,
                       AnimationPlaybackMode playbackMode) override;
    bool preloadTexture(const std::filesystem::path &file) override;
    bool preloadAnimation(const std::filesystem::path &file) override;
    void evictTexture(const std::filesystem::path &file) override;
    void evictAnimation(const std::filesystem::path &file) override;
    void clearTextureCache() override;
    void clearAnimationCache() override;

    uint32_t getAnimationDurationMs(const std::filesystem::path &file) const override;
    std::pair<uint32_t, uint32_t> getAnimationDimensions(const std::filesystem::path &file) const override;

    uint32_t width(std::string_view text) const override;
    int lineHeight() const override;
    void setFontSize(uint8_t size) override;

    SDL_Renderer *nativeRenderer() const;

private:
    struct AnimationFrame
    {
        SDL_Texture *texture = nullptr;
        uint32_t delayMs = 0;
    };

    struct AnimationData
    {
        std::vector<AnimationFrame> frames;
        uint32_t totalDurationMs = 0;
    };

    SDL_Texture *loadAndCacheTexture(const std::filesystem::path &file);
    AnimationData *loadAndCacheAnimation(const std::filesystem::path &file);
    static std::string textureKey(const std::filesystem::path &file);
    static void destroyAnimationData(AnimationData &animation);
    static uint32_t selectAnimationFrameIndex(const AnimationData &animation,
                                              uint32_t elapsedMs,
                                              AnimationPlaybackMode playbackMode,
                                              bool &shouldRender);

    SDL_Renderer *mRenderer = nullptr;
    TTF_Font *mFont = nullptr;
    std::unordered_map<std::string, SDL_Texture *> mTextureCache;
    std::unordered_map<std::string, AnimationData> mAnimationCache;
};

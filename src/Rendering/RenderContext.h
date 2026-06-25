#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include "AnimationPlaybackMode.h"
#include "theme.h"
#include "types.h"

class RenderContext
{
public:
    virtual ~RenderContext() = default;

    virtual const Window_Properties &getWindowProperties() const = 0;
    virtual const Theme &getTheme() const = 0;
    virtual void clear(RenderColor color) = 0;
    virtual void drawText(const std::string &text, int x, int y) = 0;
    virtual void drawText(const std::string &text, int x, int y, RenderColor color) = 0;
    virtual void drawRect(int x, int y, int w, int h, RenderColor color) = 0;
    virtual void drawRect(Rect rect, RenderColor color) = 0;
    virtual void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) = 0;
    virtual void loadTextureByName(float x, float y, float w, float h, std::string_view assetName, float rotation = 0.0f) = 0;
    virtual void loadAnimationByName(float x, float y, float w, float h,
                                     std::string_view assetName,
                                     uint32_t elapsedMs,
                                     AnimationPlaybackMode playbackMode,
                                     float rotation = 0.0f) = 0;
    virtual uint32_t getAnimationDurationByName(std::string_view assetName) const = 0;
    virtual std::pair<uint32_t, uint32_t> getAnimationDimensionsByName(std::string_view assetName) const = 0;
    virtual void pushClipRect(const Rect &rect) = 0;
    virtual void clearClipRect() = 0;
};
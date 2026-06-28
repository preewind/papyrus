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
    virtual void drawRainbowText(const std::string &text, int x, int y) = 0;
    virtual void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, RenderColor color) = 0;
    virtual void drawRect(Rect rect, RenderColor color) = 0;
    virtual void drawRect(float x, float y, float w, float h, RenderColor color) = 0;
    virtual void drawRect(RectF rect, RenderColor color) = 0;
    virtual void drawDottedLine(float x, float y, float length, float lineWidth, float spacing, float rectSize, const RenderColor &color) = 0;
    virtual void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) = 0;
    virtual void loadTextureByName(float x, float y, float w, float h, std::string_view assetName, float rotation = 0.0f) = 0;
    virtual void loadAnimationByName(float x, float y, float w, float h, std::string_view assetName, uint32_t elapsedMs, AnimationPlaybackMode playbackMode, float rotation = 0.0f) = 0;
    virtual uint32_t getAnimationDurationByName(std::string_view assetName) const = 0;
    virtual std::pair<uint32_t, uint32_t> getAnimationDimensionsByName(std::string_view assetName) const = 0;
    virtual void pushClipRect(const Rect &rect) = 0;
    virtual void clearClipRect() = 0;
};
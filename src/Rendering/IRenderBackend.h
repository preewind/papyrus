#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

#include "RenderTypes.h"
#include "ITextMeasurer.h"

class IRenderBackend : public ITextMeasurer
{
public:
    virtual ~IRenderBackend() = default;

    virtual void setVSync(bool enabled) = 0;
    virtual void clear(const RenderColor &color) = 0;
    virtual void present() = 0;

    virtual void fillRect(const RenderRect &rect, const RenderColor &color) = 0;
    virtual void setClipRect(const RenderRect &rect) = 0;
    virtual void clearClipRect() = 0;

    virtual void drawText(const std::string &text, int x, int y, const RenderColor &color) = 0;
    virtual void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) = 0;
    virtual bool preloadTexture(const std::filesystem::path &file) = 0;
    virtual void evictTexture(const std::filesystem::path &file) = 0;
    virtual void clearTextureCache() = 0;

    virtual int lineHeight() const = 0;
    virtual void setFontSize(uint8_t size) = 0;
};

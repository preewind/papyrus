#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

#include "AnimationPlaybackMode.h"
#include "ITextMeasurer.h"
#include "RenderContext.h"

struct FixedWidthMeasurer : public ITextMeasurer
{
    explicit FixedWidthMeasurer(uint32_t glyphWidth = 8)
        : mGlyphWidth(glyphWidth)
    {
    }

    uint32_t width(std::string_view text) const override
    {
        return static_cast<uint32_t>(text.size()) * mGlyphWidth;
    }

private:
    uint32_t mGlyphWidth;
};

struct FakeRenderContext : public RenderContext
{
    struct TextCall
    {
        std::string text;
        int x = 0;
        int y = 0;
        bool hasColor = false;
        bool isRainbow = false;
        RenderColor color{};
    };

    struct RectCall
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t w = 0;
        uint32_t h = 0;
        RenderColor color{};
    };

    struct RectCallF
    {
        float x = 0;
        float y = 0;
        float w = 0;
        float h = 0;
        RenderColor color{};
    };

    struct AnimationCall
    {
        float x = 0;
        float y = 0;
        float w = 0;
        float h = 0;
        std::string assetName;
        uint32_t elapsedMs = 0;
        AnimationPlaybackMode playbackMode = AnimationPlaybackMode::Loop;
    };

    Window_Properties windowProperties{20, 1280, 720};
    Theme theme{};
    std::vector<TextCall> textCalls;
    std::vector<RectCall> rectCalls;
    std::vector<RectCallF> rectFCalls;
    std::vector<AnimationCall> animationCalls;
    std::vector<Rect> clipRects;
    uint32_t clearClipCount = 0;

    const Window_Properties &getWindowProperties() const override
    {
        return windowProperties;
    }

    const Theme &getTheme() const override
    {
        return theme;
    }

    void clear(RenderColor color) override
    {
        (void)color;
    }

    void drawText(const std::string &text, int x, int y) override
    {
        textCalls.push_back(TextCall{text, x, y, false, false, {}});
    }

    void drawRainbowText(const std::string &text, int x, int y) override
    {
        textCalls.push_back(TextCall{text, x, y, false, true, {}});
    }

    void drawText(const std::string &text, int x, int y, RenderColor color) override
    {
        textCalls.push_back(TextCall{text, x, y, true, false, color});
    }

    void drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, RenderColor color) override
    {
        rectCalls.push_back(RectCall{x, y, w, h, color});
    }

    void drawRect(Rect rect, RenderColor color) override
    {
        drawRect(rect.x, rect.y, rect.w, rect.h, color);
    }
    void drawRect(float x, float y, float w, float h, RenderColor color) override
    {
        rectFCalls.push_back(RectCallF{x, y, w, h, color});
    }
    void drawRect(RectF rect, RenderColor color) override
    {
        drawRect(rect.x, rect.y, rect.w, rect.h, color);
    }

    void drawDottedLine(float x, float y, float length, float lineWidth, float spacing, float rectSize, const RenderColor &color) override
    {
        (void)x;
        (void)y;
        (void)length;
        (void)lineWidth;
        (void)spacing;
        (void)rectSize;
        (void)color;
    }

    void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) override
    {
        (void)file;
        (void)x;
        (void)y;
        (void)w;
        (void)h;
    }

    void loadTextureByName(float x, float y, float w, float h, std::string_view assetName, float rotation = 0.0f) override
    {
        (void)assetName;
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        (void)rotation;
    }

    void loadAnimationByName(float x, float y, float w, float h,
                             std::string_view assetName,
                             uint32_t elapsedMs,
                             AnimationPlaybackMode playbackMode,
                             float rotation = 0.0f) override
    {
        (void)rotation;
        animationCalls.push_back(AnimationCall{x, y, w, h, std::string(assetName), elapsedMs, playbackMode});
    }
    uint32_t getAnimationDurationByName(std::string_view assetName) const override
    {
        (void)assetName;
        return 0;
    }
    std::pair<uint32_t, uint32_t> getAnimationDimensionsByName(std::string_view assetName) const override
    {
        (void)assetName;
        return {0, 0};
    }

    void pushClipRect(const Rect &rect) override
    {
        clipRects.push_back(rect);
    }

    void clearClipRect() override
    {
        ++clearClipCount;
    }
};

inline SDL_Event makeKeyDown(SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE)
{
    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = key;
    event.key.mod = mod;
    return event;
}

inline SDL_Event makeTextInput(const char *text)
{
    SDL_Event event{};
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.text = text;
    return event;
}

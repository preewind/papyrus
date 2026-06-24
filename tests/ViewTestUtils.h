#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

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
        RenderColor color{};
    };

    struct RectCall
    {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        RenderColor color{};
    };

    Window_Properties windowProperties{20, 1280, 720};
    Theme theme{};
    std::vector<TextCall> textCalls;
    std::vector<RectCall> rectCalls;
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

    void clear(RenderColor color) override{
        (void) color;
    }

    void drawText(const std::string &text, int x, int y) override
    {
        textCalls.push_back(TextCall{text, x, y, false, {}});
    }

    void drawText(const std::string &text, int x, int y, RenderColor color) override
    {
        textCalls.push_back(TextCall{text, x, y, true, color});
    }

    void drawRect(int x, int y, int w, int h, RenderColor color) override
    {
        rectCalls.push_back(RectCall{x, y, w, h, color});
    }

    void drawRect(Rect rect, RenderColor color) override
    {
        drawRect(static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.w), static_cast<int>(rect.h), color);
    }

    void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) override
    {
        (void)file;
        (void)x;
        (void)y;
        (void)w;
        (void)h;
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

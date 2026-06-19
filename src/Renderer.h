#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "types.h"
#include "theme.h"
#include "RenderContext.h"

class Cursor;
class Editor;
class Selection;
class Terminal;
class IRenderBackend;
class ITextMeasurer;


class Renderer : public RenderContext
{

public:
    Renderer(SDL_Window *window, uint8_t fontSize);
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void clear();
    int getLineHeight() const;
    const SDL_Properties &getSDL_Properties() const override;
    const Theme &getTheme() const override;

    const ITextMeasurer &getTextMeasurer() const;
    void drawText(const std::string &text, int x, int y) override;
    void drawText(const std::string &text, int x, int y, RenderColor color) override;
    void drawRect(int x, int y, int w, int h, RenderColor color) override;
    void drawRect(Rect rect, RenderColor color) override;
    void pushClipRect(const Rect &rect) override;
    void clearClipRect() override;
    void present();

    void onResize(uint32_t w, uint32_t h);

    void setFontSize(uint8_t fontSize);

private:
    std::unique_ptr<IRenderBackend> mBackend;
    Theme mTheme;
    SDL_Properties mLayout;
};
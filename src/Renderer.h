#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "types.h"
#include "theme.h"
#include "CursorBlinker.h"
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
    Renderer(SDL_Window *window);
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void clear();
    int getLineHeight() const;
    const SDL_Properties &getSDL_Properties() const override;
    const Theme &getTheme() const override;

    const ITextMeasurer &getTextMeasurer() const;
    const CursorBlinker &getCursorBlinker() const override;
    void drawText(const std::string &text, int x, int y) override;
    void drawText(const std::string &text, int x, int y, RenderColor color) override;
    void drawRect(int x, int y, int w, int h, RenderColor color) override;
    void drawRect(Rect rect, RenderColor color) override;
    void pushClipRect(const Rect &rect) override;
    void clearClipRect() override;
    void updateEditor(Editor &editor);
    void present();

    void onResize(uint32_t w, uint32_t h);

    void setFontSize();
    void handlePlus();
    void handleMinus();

private:
    std::unique_ptr<IRenderBackend> mBackend;
    uint8_t mFontSize = 20;
    CursorBlinker mCursorBlinker;
    Theme mTheme;
    SDL_Properties mLayout;
};
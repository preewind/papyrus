#pragma once

#include <cstdint>

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
    int getLineHeight() const;
    const Window_Properties &getWindowProperties() const override;
    const Theme &getTheme() const override;

    void drawText(const std::string &text, int x, int y) override;
    void drawText(const std::string &text, int x, int y, RenderColor color) override;
    void drawRect(int x, int y, int w, int h, RenderColor color) override;
    void drawRect(Rect rect, RenderColor color) override;
    void loadTexture(float x, float y, float w, float h, const std::filesystem::path &file) override;
    void pushClipRect(const Rect &rect) override;
    void clearClipRect() override;
    void present();

    void onResize(uint32_t w, uint32_t h);

    void setFontSize(uint8_t fontSize);

private:
    IRenderBackend *mBackend = nullptr;
    const Theme *mTheme = nullptr;
    Window_Properties mLayout;
};
#pragma once

#include "Screensaver.h"
#include "RenderContext.h"

class ScreensaverView {
public:
    void render(RenderContext &renderContext, const Screensaver& screensaver, const Window_Properties &windowProps);
private:
    void renderLogo(RenderContext &renderContext, const Screensaver& screensaver, const Window_Properties &windowProps);
};
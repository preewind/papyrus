#pragma once

#include "Screensaver.h"
#include "RenderContext.h"

class ScreensaverView
{
public:
    void render(RenderContext &renderContext, const Screensaver &screensaver);
};
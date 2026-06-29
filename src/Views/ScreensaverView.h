#pragma once

#include "Screensaver.h"
#include "RenderContext.h"

class TextLayout;

class ScreensaverView
{
public:
    void render(RenderContext &renderContext, const TextLayout &textLayout, const Screensaver &screensaver);
};
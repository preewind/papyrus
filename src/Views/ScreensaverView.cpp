#include "ScreensaverView.h"

void ScreensaverView::render(RenderContext &renderContext, const Screensaver &screensaver, const Window_Properties &windowProps)
{
    if(screensaver.isInactive()){
        renderLogo(renderContext, screensaver, windowProps);
    }
}

void ScreensaverView::renderLogo(RenderContext &renderContext, const Screensaver &screensaver, const Window_Properties &windowProps)
{
    (void)screensaver;
    uint32_t size = 50;
    Rect logo{windowProps.totalWindowWidth / 2 - size / 2, windowProps.totalWindowHeight / 2 - size / 2, size, size};
    renderContext.drawRect(logo, {255, 0, 0, 255});
}
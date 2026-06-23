#include "ScreensaverView.h"

void ScreensaverView::render(RenderContext &renderContext, const Screensaver &screensaver)
{
    if (screensaver.isInactive())
    {
        renderLogo(renderContext, screensaver);
    }
}

void ScreensaverView::renderLogo(RenderContext &renderContext, const Screensaver &screensaver)
{
    const Logo &logo = screensaver.getLogo();
    renderContext.drawRect(logo.x, logo.y, logo.w, logo.h, {255, 0, 0, 255});
}
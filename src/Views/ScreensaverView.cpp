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
    renderContext.loadTexture(logo.x, logo.y, logo.w, logo.h, "./assets/dvd_logo.png");
}
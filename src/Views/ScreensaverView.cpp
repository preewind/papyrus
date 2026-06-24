#include "ScreensaverView.h"

void ScreensaverView::render(RenderContext &renderContext, const Screensaver &screensaver)
{
    if (screensaver.isInactive())
    {
        renderContext.clear({34,35,46,255});
        renderLogo(renderContext, screensaver);
        if (screensaver.isSuccess())
        {
            renderSuccess(renderContext, screensaver);
        }
    }
}

void ScreensaverView::renderLogo(RenderContext &renderContext, const Screensaver &screensaver)
{
    const Logo &logo = screensaver.getLogo();
    renderContext.loadTexture(logo.x, logo.y, logo.w, logo.h, "./assets/dvd_logo.png");
}

void ScreensaverView::renderSuccess(RenderContext &renderContext, const Screensaver &screensaver)
{
    const SuccessAnimation &success = screensaver.getSuccessAnimation();
    renderContext.loadTexture(success.currentX,success.currentY, success.w, success.h, "./assets/thugglasses.gif");
}

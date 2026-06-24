#include <random>

#include "ScreensaverAssets.h"
#include "ScreensaverView.h"

void ScreensaverView::render(RenderContext &renderContext, const Screensaver &screensaver)
{
    if (screensaver.isInactive())
    {
        renderContext.clear({34, 35, 46, 255});
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
    renderContext.loadTextureByName(logo.x, logo.y, logo.w, logo.h, ScreensaverAssets::Logo);
}

void ScreensaverView::renderSuccess(RenderContext &renderContext, const Screensaver &screensaver)
{
    const SuccessAnimation &success = screensaver.getSuccessAnimation();
    renderContext.loadTextureByName(success.currentX, success.currentY, success.w, success.h, ScreensaverAssets::Success);
    const auto &markers = screensaver.getMarkers();
    const uint32_t frameTime = screensaver.getFrameTimeMs();
    if (!success.active)
    {
        for (const auto& marker : markers)
        {
            if (frameTime >= marker.startTime && frameTime < marker.startTime + marker.duration)
            {
                renderContext.loadTextureByName(marker.x, marker.y, 50, 50, ScreensaverAssets::HitMarker);
            }
        }
    }
}

#include "ScreensaverAssets.h"
#include "ScreensaverView.h"
#include "logger.h"

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

    if (!success.active)
    {
        const uint32_t frameTime = screensaver.getFrameTimeMs();
        for (const auto &group : screensaver.getEffects())
        {
            for (const auto &e : group.instances)
            {
                if (frameTime < e.startTime || frameTime >= e.startTime + e.duration)
                    continue;

                const uint32_t elapsed = frameTime - e.startTime;
                if (group.def.isAnimation)
                    renderContext.loadAnimationByName(e.x, e.y, e.w, e.h, group.def.assetName, elapsed, AnimationPlaybackMode::HideAfterEnd);
                else
                    renderContext.loadTextureByName(e.x, e.y, e.w, e.h, group.def.assetName);
            }
        }
    }
}

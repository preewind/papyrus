#include "ScreensaverAssets.h"
#include "ScreensaverView.h"
#include "random.h"

void ScreensaverView::render(RenderContext &renderContext, const Screensaver &screensaver)
{
    if (screensaver.isInactive())
    {
        renderDvdScreensaver(renderContext, screensaver.getDvdScreensaverConst(), screensaver.getFrameTimeMs());
    }
}

void ScreensaverView::renderDvdScreensaver(RenderContext &renderContext, const DvdScreensaver &dvdScreensaver, uint32_t frameTime)
{

    renderContext.clear({22, 22, 22, 255});
    renderLogo(renderContext, dvdScreensaver);
    if (dvdScreensaver.isSuccess())
    {
        renderSuccess(renderContext, dvdScreensaver, frameTime);
    }
}

void ScreensaverView::renderLogo(RenderContext &renderContext, const DvdScreensaver &dvdScreensaver)
{
    const Logo &logo = dvdScreensaver.getLogo();
    renderContext.loadTextureByName(logo.x, logo.y, logo.w, logo.h, ScreensaverAssets::Logo);
}

void ScreensaverView::renderSuccess(RenderContext &renderContext, const DvdScreensaver &dvdScreensaver, uint32_t frameTime)
{
    const SuccessAnimation &success = dvdScreensaver.getSuccessAnimation();
    renderContext.loadTextureByName(success.currentX, success.currentY, success.w, success.h, ScreensaverAssets::Success);

    if (!success.active)
    {
        for (const auto &group : dvdScreensaver.getEffects())
        {
            for (const auto &e : group.instances)
            {
                if (frameTime < e.startTime || frameTime >= e.startTime + e.duration)
                    continue;

                const uint32_t elapsed = frameTime - e.startTime;
                const std::string_view assetName = e.instanceAssetName.empty() ? group.def.assetName : e.instanceAssetName;
                const float jitter = group.def.jitterAmplitude;
                const float renderX = jitter > 0.0f ? e.x + Random::get_float(-jitter, jitter) : e.x;
                const float renderY = jitter > 0.0f ? e.y + Random::get_float(-jitter, jitter) : e.y;

                if (group.def.isAnimation)
                    renderContext.loadAnimationByName(renderX, renderY, e.w, e.h, assetName, elapsed, AnimationPlaybackMode::HideAfterEnd, e.rotation);
                else
                    renderContext.loadTextureByName(renderX, renderY, e.w, e.h, assetName, e.rotation);
            }
        }
    }
}

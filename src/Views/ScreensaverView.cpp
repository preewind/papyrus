#include "ScreensaverView.h"
#include "Screensaver.h"
#include "RenderContext.h"

void ScreensaverView::render(RenderContext &renderContext, const Screensaver &screensaver)
{
    if (screensaver.isInactive())
    {
        const IScreensaverLogic *activeScene = screensaver.getActiveScene();
        if (activeScene != nullptr)
        {
            activeScene->render(renderContext, screensaver.getFrameTimeMs());
        }
    }
}

#include "ScreensaverView.h"
#include "Screensaver.h"
#include "RenderContext.h"
#include "TextLayout.h"

void ScreensaverView::render(RenderContext &renderContext, const TextLayout &textLayout, const Screensaver &screensaver)
{
    if (screensaver.isInactive())
    {
        const IScreensaverLogic *activeScene = screensaver.getActiveScene();
        if (activeScene != nullptr)
        {
            activeScene->render(renderContext, textLayout, screensaver.getFrameTimeMs());
        }
    }
}

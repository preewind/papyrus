#include <gtest/gtest.h>

#include "Screensaver.h"
#include "ScreensaverAssets.h"
#include "ScreensaverView.h"
#include "ViewTestUtils.h"

TEST(ScreensaverViewTests, SuccessRenderRequestsExplosionAnimationWithHideAfterEnd)
{
    Screensaver screensaver;
    Window_Properties windowProps{20, 1280, 720};

    // Run until success is reached so renderSuccess path is active.
    for (int i = 0; i < 200000 && !screensaver.isSuccess(); ++i)
    {
        screensaver.runScreensaver(windowProps);
    }

    screensaver.resetTimer();
    screensaver.updateScreensaver();
    for (int i = 0; i < 5; ++i)
    {
        screensaver.updateScreensaver();
    }
    // Force inactive screensaver render path for view rendering.
    while (!screensaver.isInactive())
    {
        screensaver.updateScreensaver();
    }

    ASSERT_TRUE(screensaver.isSuccess());

    FakeRenderContext renderContext;
    ScreensaverView view;
    view.render(renderContext, screensaver);

    ASSERT_FALSE(renderContext.animationCalls.empty());
    const auto &call = renderContext.animationCalls.front();
    EXPECT_EQ(call.assetName, ScreensaverAssets::Explosion);
    EXPECT_EQ(call.playbackMode, AnimationPlaybackMode::HideAfterEnd);
}

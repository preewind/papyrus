#include <gtest/gtest.h>

#include <algorithm>

#include "PongScreensaver.h"
#include "TextLayout.h"
#include "ViewTestUtils.h"

namespace
{
bool containsText(const FakeRenderContext &renderContext, const std::string &text)
{
    return std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [&](const FakeRenderContext::TextCall &call)
                       { return call.text == text; });
}
}

TEST(PongScreensaverTests, DemoShowsHintAndCtrlEnterOpensMenu)
{
    PongScreensaver screensaver;

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    screensaver.update(renderContext.windowProperties, 0, 0.016f);
    screensaver.render(renderContext, textLayout, 0);

    EXPECT_TRUE(containsText(renderContext, "Demo mode"));
    EXPECT_TRUE(containsText(renderContext, "Press Ctrl+Return to play"));

    screensaver.handleKey(makeKeyDown(SDLK_RETURN, SDL_KMOD_CTRL));
    renderContext.textCalls.clear();

    screensaver.render(renderContext, textLayout, 0);

    EXPECT_TRUE(containsText(renderContext, "Pong"));
    EXPECT_TRUE(containsText(renderContext, "Level"));
    EXPECT_TRUE(containsText(renderContext, "Gamemode"));
}

TEST(PongScreensaverTests, MenuRowsAlignOnColon)
{
    PongScreensaver screensaver;

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    screensaver.handleKey(makeKeyDown(SDLK_RETURN, SDL_KMOD_CTRL));
    screensaver.render(renderContext, textLayout, 0);

    std::vector<int> colonXs;
    for (const FakeRenderContext::TextCall &call : renderContext.textCalls)
    {
        if (call.text == ":")
        {
            colonXs.push_back(call.x);
        }
    }

    ASSERT_EQ(colonXs.size(), 2u);
    EXPECT_EQ(colonXs[0], colonXs[1]);
}

TEST(PongScreensaverTests, EscapePausesRunningMatch)
{
    PongScreensaver screensaver;

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    screensaver.handleKey(makeKeyDown(SDLK_RETURN, SDL_KMOD_CTRL));
    screensaver.handleKey(makeKeyDown(SDLK_RETURN));
    screensaver.update(renderContext.windowProperties, 0, 0.016f);
    screensaver.handleKey(makeKeyDown(SDLK_ESCAPE));
    screensaver.render(renderContext, textLayout, 0);

    EXPECT_TRUE(containsText(renderContext, "Pause"));
    EXPECT_TRUE(containsText(renderContext, "<Resume>"));
    EXPECT_TRUE(containsText(renderContext, "<Restart Match>") || containsText(renderContext, "<Main Menu>") || containsText(renderContext, "<Resume>"));
}

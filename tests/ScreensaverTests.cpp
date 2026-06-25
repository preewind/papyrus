#include <gtest/gtest.h>

#include <algorithm>

#include "Core/types.h"
#include "Screensaver.h"
#include "ScreensaverAssets.h"

namespace
{
const EffectDef *findEffectDef(const Screensaver &screensaver, std::string_view assetName)
{
    const auto &effects = screensaver.getEffects();
    const auto it = std::find_if(effects.begin(), effects.end(), [assetName](const EffectGroup &group)
                                 { return group.def.assetName == assetName; });

    return it == effects.end() ? nullptr : &it->def;
}

const EffectGroup *findEffectGroup(const Screensaver &screensaver, std::string_view assetName)
{
    const auto &effects = screensaver.getEffects();
    const auto it = std::find_if(effects.begin(), effects.end(), [assetName](const EffectGroup &group)
                                 { return group.def.assetName == assetName; });

    return it == effects.end() ? nullptr : &*it;
}
}

TEST(ScreensaverTests, ConstructorDefinesExpectedEffectGroups)
{
    Screensaver screensaver;

    const EffectDef *marker = findEffectDef(screensaver, ScreensaverAssets::HitMarker);
    ASSERT_NE(marker, nullptr);
    EXPECT_FALSE(marker->isAnimation);
    EXPECT_EQ(marker->count, 200u);
    EXPECT_FLOAT_EQ(marker->w, 50.0f);
    EXPECT_FLOAT_EQ(marker->h, 50.0f);
    EXPECT_EQ(marker->duration, 500u);
    EXPECT_EQ(marker->maxOffsetMs, 600u);
    EXPECT_EQ(marker->positionMode, EffectPositionMode::Random);

    const EffectDef *wow = findEffectDef(screensaver, ScreensaverAssets::Wow);
    ASSERT_NE(wow, nullptr);
    EXPECT_TRUE(wow->isAnimation);
    EXPECT_EQ(wow->count, 1u);
    EXPECT_FLOAT_EQ(wow->w, 0.0f);
    EXPECT_FLOAT_EQ(wow->h, 0.0f);
    EXPECT_EQ(wow->duration, 0u);
    EXPECT_EQ(wow->positionMode, EffectPositionMode::Centered);
    EXPECT_FLOAT_EQ(wow->dimensionScale, 2.0f);
}

TEST(ScreensaverTests, ResolveEffectDefAppliesDimensionScaleToAutoSizedEffects)
{
    Screensaver screensaver;

    screensaver.resolveEffectDef(ScreensaverAssets::Wow, 1350, 498.0f, 280.0f);

    const EffectDef *wow = findEffectDef(screensaver, ScreensaverAssets::Wow);
    ASSERT_NE(wow, nullptr);
    EXPECT_EQ(wow->duration, 1350u);
    EXPECT_FLOAT_EQ(wow->w, 996.0f);
    EXPECT_FLOAT_EQ(wow->h, 560.0f);
}

TEST(ScreensaverTests, ResolveEffectDefKeepsExplicitDimensions)
{
    Screensaver screensaver;

    screensaver.resolveEffectDef(ScreensaverAssets::Explosion, 800, 64.0f, 64.0f);

    const EffectDef *explosion = findEffectDef(screensaver, ScreensaverAssets::Explosion);
    ASSERT_NE(explosion, nullptr);
    EXPECT_EQ(explosion->duration, 800u);
    EXPECT_FLOAT_EQ(explosion->w, 420.0f);
    EXPECT_FLOAT_EQ(explosion->h, 420.0f);
}

TEST(ScreensaverTests, RunScreensaverInitializesAndMovesLogo)
{
    Screensaver screensaver;
    Window_Properties windowProps{20, 1280, 720};

    screensaver.runScreensaver(windowProps);

    const Logo &logo = screensaver.getLogo();
    EXPECT_EQ(logo.w, 386u);
    EXPECT_EQ(logo.h, 180u);
    EXPECT_EQ(logo.dx, 3);
    EXPECT_EQ(logo.dy, 3);
    EXPECT_EQ(logo.x, 441);
    EXPECT_EQ(logo.y, 273);
}

TEST(ScreensaverTests, SuccessSpawnPopulatesConfiguredEffectInstances)
{
    Screensaver screensaver;
    Window_Properties windowProps{20, 1280, 720};

    screensaver.resolveEffectDef(ScreensaverAssets::Explosion, 500, 420.0f, 420.0f);
    screensaver.resolveEffectDef(ScreensaverAssets::Wow, 1350, 498.0f, 280.0f);

    for (int i = 0; i < 200000 && !screensaver.isSuccess(); ++i)
    {
        screensaver.runScreensaver(windowProps);
    }

    ASSERT_TRUE(screensaver.isSuccess());

    for (const auto &group : screensaver.getEffects())
    {
        ASSERT_EQ(group.instances.size(), group.def.count);
        for (const auto &effect : group.instances)
        {
            EXPECT_FLOAT_EQ(effect.w, group.def.w);
            EXPECT_FLOAT_EQ(effect.h, group.def.h);
            EXPECT_EQ(effect.duration, group.def.duration);
            EXPECT_LE(effect.startOffset, group.def.maxOffsetMs);

            if (group.def.positionMode == EffectPositionMode::Centered)
            {
                EXPECT_FLOAT_EQ(effect.x, (windowProps.totalWindowWidth - group.def.w) / 2.0f);
                EXPECT_FLOAT_EQ(effect.y, (windowProps.totalWindowHeight - group.def.h) / 2.0f);
            }
            else
            {
                EXPECT_GE(effect.x, 0.0f);
                EXPECT_GE(effect.y, 0.0f);
                EXPECT_LE(effect.x, windowProps.totalWindowWidth - group.def.w);
                EXPECT_LE(effect.y, windowProps.totalWindowHeight - group.def.h);
            }
        }
    }

    const EffectGroup *wow = findEffectGroup(screensaver, ScreensaverAssets::Wow);
    ASSERT_NE(wow, nullptr);
    ASSERT_EQ(wow->instances.size(), 1u);
    EXPECT_FLOAT_EQ(wow->instances.front().w, 996.0f);
    EXPECT_FLOAT_EQ(wow->instances.front().h, 560.0f);
}
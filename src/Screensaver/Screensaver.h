#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_events.h>
#include <optional>

struct Window_Properties;

struct Vec2
{
    float x, y;
};

struct Logo
{
    int x, y;
    int dx, dy;
    uint32_t w, h;
};

struct SuccessAnimation
{
    float startX, startY;
    float currentX, currentY;
    float endX, endY;
    bool active = true;
    uint32_t w, h;
    float speedPixelsPerSecond = 414.0f;
};

struct SuccessEffect
{
    float x, y, w, h;
    uint32_t startTime = 0;
    uint32_t startOffset = 0;
    uint32_t duration = 0;
    float rotation = 0.0f;
    std::string_view instanceAssetName; // non-empty when spawned from an assetVariants group
};

enum class EffectPositionMode
{
    Random,
    RandomGrid,
    Centered
};

struct EffectDef
{
    std::string_view assetName; // primary asset (also used for dimension/duration resolution)
    std::string_view assetPath;
    bool isAnimation;
    uint32_t count;
    float w, h;        // 0 = resolve from asset
    uint32_t duration; // ms, 0 = resolve from asset
    uint32_t maxOffsetMs;
    EffectPositionMode positionMode;
    float dimensionScale = 1.0f;                     // only applied when w/h are auto-resolved
    float maxRotationDeg = 0.0f;                     // if > 0, each instance gets a random tilt in [-max, +max]
    float jitterAmplitude = 0.0f;                    // if > 0, each frame applies a random ±N px position offset
    std::vector<std::string_view> assetVariants;     // when non-empty, each instance picks one of these
    std::vector<std::string_view> assetVariantPaths; // parallel paths for assetVariants
};

struct EffectGroup
{
    EffectDef def;
    std::vector<SuccessEffect> instances;
};

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    void runScreensaver(const Window_Properties &windowProps);
    void resolveEffectDef(std::string_view assetName, uint32_t duration, float w, float h);
    void resolveEffectVariantDef(std::string_view assetName, uint32_t duration, float w, float h, float dimensionScale = 1.0f);
    bool isSuccess() const;
    const SuccessAnimation &getSuccessAnimation() const;
    const std::vector<EffectGroup> &getEffects() const;
    uint32_t getFrameTimeMs() const;
    bool isInactive() const;
    void resetTimer();
    void setIntervalSeconds(uint64_t interval);
    const Logo &getLogo() const;
    void handleKey(const SDL_Event &event);

private:
    struct EffectResolvedDef
    {
        uint32_t duration = 0;
        float w = 0.0f;
        float h = 0.0f;
    };

    void initializeLogo();
    void startSuccessScene(const Window_Properties &windowProps, uint32_t nowMs);
    void spawnSuccessEffects(const Window_Properties &windowProps);
    void runSuccessScene(uint32_t nowMs, float deltaSeconds);
    bool areAllEffectsFinished(uint32_t nowMs) const;
    Vec2 getRandomGridPos(uint32_t cellX, uint32_t cellY, uint32_t count, uint32_t screenWidth, uint32_t screenHeight, float spriteWidth, float spriteHeight) const;

    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 10;
    bool mInactive = false;
    bool mInitialized = false;
    bool mSuccess = false;
    uint32_t mLastFrameTimeMs = 0;
    uint32_t mFrameTimeMs = 0;
    std::vector<EffectGroup> mEffects;
    std::unordered_map<std::string, EffectResolvedDef> mResolvedVariantDefs;
    Logo mLogo;
    SuccessAnimation mSuccessAnimation;
};
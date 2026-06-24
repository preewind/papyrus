#pragma once

#include <stdint.h>
#include <string_view>
#include <vector>
#include <SDL3/SDL_events.h>

#include "types.h"

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
    uint32_t startTime   = 0;
    uint32_t startOffset = 0;
    uint32_t duration    = 0;
};

enum class EffectPositionMode { Random, Centered };

struct EffectDef
{
    std::string_view assetName;
    std::string_view assetPath;
    bool             isAnimation;
    size_t           count;
    float            w, h;              // 0 = resolve from asset
    uint32_t         duration;          // ms, 0 = resolve from asset
    uint32_t         maxOffsetMs;
    EffectPositionMode positionMode;
    float            dimensionScale = 1.0f; // only applied when w/h are auto-resolved
};

struct EffectGroup
{
    EffectDef                def;
    std::vector<SuccessEffect> instances;
};

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    void runScreensaver(const Window_Properties &windowProps);
    void runSuccessScene(uint32_t nowMs, float deltaSeconds);
    void resolveEffectDef(std::string_view assetName, uint32_t duration, float w, float h);
    bool isSuccess() const;
    const SuccessAnimation &getSuccessAnimation() const;
    const std::vector<EffectGroup> &getEffects() const;
    uint32_t getFrameTimeMs() const;
    bool isInactive() const;
    void resetTimer();
    const Logo &getLogo() const;
    bool canHitCorner(const Window_Properties &windowProps);
    void handleKey(const SDL_Event &event);

private:
    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 3;
    bool mInactive = false;
    bool mInitialized = false;
    bool mSuccess = false;
    uint32_t mLastFrameTimeMs = 0;
    uint32_t mFrameTimeMs = 0;
    uint32_t mSuccessStartTimeMs = 0;
    std::vector<EffectGroup> mEffects;
    Logo mLogo;
    SuccessAnimation mSuccessAnimation;
};
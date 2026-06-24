#pragma once

#include <stdint.h>
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

struct HitMarker
{
    int x, y;
    uint32_t startTime;
    uint32_t startOffset;
    uint32_t duration;
};

struct Explosion
{
    int x, y;
    int w, h;
    uint32_t startTime;
    uint32_t startOffset;
    uint32_t duration;
};

struct Wow
{
    int x, y;
    int w, h;
    uint32_t startTime;
    uint32_t startOffset;
};

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    void runScreensaver(const Window_Properties &windowProps);
    void runSuccessScene(uint32_t nowMs, float deltaSeconds);
    bool isSuccess() const;
    const SuccessAnimation &getSuccessAnimation() const;
    const std::vector<HitMarker> &getMarkers() const;
    const std::vector<Explosion> &getExplosions() const;
    const Wow &getWow() const;
    uint32_t getFrameTimeMs() const;
    uint32_t getSuccessElapsedMs() const;
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
    std::vector<HitMarker> mMarkers;
    std::vector<Explosion> mExplosions;
    Wow mWow;
    Logo mLogo;
    SuccessAnimation mSuccessAnimation;
};
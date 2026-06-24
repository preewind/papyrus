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
    int startX, startY;
    int currentX, currentY;
    int endX, endY;
    uint32_t w, h;
};

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    void runScreensaver(const Window_Properties &windowProps);
    void runSuccessScene();
    bool isSuccess() const;
    const SuccessAnimation &getSuccessAnimation() const;
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
    Logo mLogo;
    SuccessAnimation mSuccessAnimation;
};
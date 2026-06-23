#pragma once

#include <stdint.h>

#include "types.h"

struct Logo{
    int x, y;
    int dx, dy;
    uint32_t w, h;
};

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver(const Window_Properties& windowProps);
    void runScreensaver(const Window_Properties& windowProps);
    bool isInactive() const;
    const Logo& getLogo() const;
    bool canHitCorner(const Window_Properties& windowProps);

private:
    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 3;
    bool mInactive = false;
    bool mInitialized = false;
    Logo mLogo;
};
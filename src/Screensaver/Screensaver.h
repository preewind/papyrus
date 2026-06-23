#pragma once

#include <stdint.h>

#include "types.h"

struct Logo{
    float x, y;
    float dx, dy;
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

private:
    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 5;
    bool mInactive = false;
    bool mInitialized = false;
    Logo mLogo;
};
#pragma once

#include <stdint.h>

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    void runScreensaver();
    bool isInactive() const;

private:
    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 5;
    bool mInactive = false;
};
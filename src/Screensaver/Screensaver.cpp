#include <SDL3/SDL_timer.h>

#include "Screensaver.h"
#include "logger.h"

Screensaver::Screensaver()
{
    mInactivityTimer = SDL_GetTicks();
}

void Screensaver::updateScreensaver()
{
    uint64_t currTime = SDL_GetTicks();
    uint64_t timeToInactivity = (currTime - mInactivityTimer) / SDL_MS_PER_SECOND;
    if (timeToInactivity >= mInactivityInterval)
    {
        runScreensaver();
        mInactive = true;
    }
    LOG_DEBUG() << "Time to inactivity: " << timeToInactivity;
}

void Screensaver::runScreensaver()
{
    // TODO: update position
}

bool Screensaver::isInactive() const
{
    return mInactive;
}

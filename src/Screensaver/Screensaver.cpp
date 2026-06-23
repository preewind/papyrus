#include <SDL3/SDL_timer.h>

#include "Screensaver.h"
#include "logger.h"

Screensaver::Screensaver()
{
    mInactivityTimer = SDL_GetTicks();
}

void Screensaver::updateScreensaver(const Window_Properties& windowProps)
{
    uint64_t currTime = SDL_GetTicks();
    uint64_t timeToInactivity = (currTime - mInactivityTimer) / SDL_MS_PER_SECOND;
    if (timeToInactivity >= mInactivityInterval)
    {
        runScreensaver(windowProps);
        mInactive = true;
    }
    LOG_DEBUG() << "Time to inactivity: " << timeToInactivity;
}

void Screensaver::runScreensaver(const Window_Properties& windowProps)
{
    if (!mInitialized) {
        mLogo.w = 50;
        mLogo.h = 50;
        mLogo.x = (windowProps.totalWindowWidth - mLogo.w) / 2.0f;
        mLogo.y = (windowProps.totalWindowHeight - mLogo.h) / 2.0f;
        mLogo.dx = 1.0f;
        mLogo.dy = 1.0f;
        mInitialized = true;
    }

    mLogo.x += mLogo.dx;
    mLogo.y += mLogo.dy;
}

bool Screensaver::isInactive() const
{
    return mInactive;
}

const Logo &Screensaver::getLogo() const
{
    return mLogo;
}

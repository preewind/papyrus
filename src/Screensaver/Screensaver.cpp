#include <SDL3/SDL_timer.h>
#include <cmath>
#include <numeric>

#include "Screensaver.h"
#include "logger.h"

Screensaver::Screensaver()
{
    mInactivityTimer = SDL_GetTicks();
}

void Screensaver::updateScreensaver(const Window_Properties &windowProps)
{
    uint64_t currTime = SDL_GetTicks();
    uint64_t timeToInactivity = (currTime - mInactivityTimer) / SDL_MS_PER_SECOND;
    if (timeToInactivity >= mInactivityInterval)
    {
        runScreensaver(windowProps);
        mInactive = true;
    }
}

void Screensaver::runScreensaver(const Window_Properties &windowProps)
{
    if (!mInitialized)
    {
        mLogo.w = 386;
        mLogo.h = 180;
        //mLogo.x = (windowProps.totalWindowWidth - mLogo.w) / 2;
        mLogo.x = 438;
        //mLogo.y = (windowProps.totalWindowHeight - mLogo.h) / 2;
        mLogo.y = 270;
        mLogo.dx = 3;
        mLogo.dy = 3;
        mInitialized = true;
        LOG_DEBUG() << "Can the logo hit the perfect corner? " << canHitCorner(windowProps);
    }

    mLogo.x += mLogo.dx;
    mLogo.y += mLogo.dy;

    float maxX = windowProps.totalWindowWidth - mLogo.w;
    if (mLogo.x >= maxX)
    {
        mLogo.x = maxX;
        mLogo.dx *= -1;
    }
    else if (mLogo.x <= 0)
    {
        mLogo.x = 0;
        mLogo.dx *= -1;
    }
    float maxY = windowProps.totalWindowHeight - mLogo.h;
    if (mLogo.y >= maxY)
    {
        mLogo.y = maxY;
        mLogo.dy *= -1;
    }
    else if (mLogo.y <= 0)
    {
        mLogo.y = 0;
        mLogo.dy *= -1;
    }
}

bool Screensaver::isInactive() const
{
    return mInactive;
}

const Logo &Screensaver::getLogo() const
{
    return mLogo;
}

bool Screensaver::canHitCorner(const Window_Properties &windowProps)
{
    int xMax = windowProps.totalWindowWidth - mLogo.w;
    int yMax = windowProps.totalWindowHeight - mLogo.h;

    int horizontal_factor = yMax * std::abs(mLogo.dx);
    int vertical_factor = xMax * std::abs(mLogo.dy);

    return std::gcd(horizontal_factor, vertical_factor) > 1;
}

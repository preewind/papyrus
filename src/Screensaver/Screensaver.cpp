#include <SDL3/SDL_timer.h>
#include <cmath>
#include <numeric>

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
        mInactive = true;
    }
}

void Screensaver::runScreensaver(const Window_Properties &windowProps)
{
    if (mSuccess)
    {
        runSuccessScene();
        return;
    }
    if (!mInitialized)
    {
        mLogo.w = 386;
        mLogo.h = 180;
        // mLogo.x = (windowProps.totalWindowWidth - mLogo.w) / 2;
        mLogo.x = 438;
        // mLogo.y = (windowProps.totalWindowHeight - mLogo.h) / 2;
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

    bool upperLeft = mLogo.x == 0 && mLogo.y == 0;
    bool upperRight = mLogo.x == maxX && mLogo.y == 0;
    bool lowerLeft = mLogo.x == 0 && mLogo.y == maxY;
    bool lowerRight = mLogo.x == maxX && mLogo.y == maxY;

    if (upperLeft || upperRight || lowerLeft || lowerRight)
    {
        mSuccess = true;
        mSuccessAnimation.endX = mLogo.x;
        mSuccessAnimation.endY = mLogo.y - mLogo.h / 5;
        mSuccessAnimation.w = mLogo.w;
        mSuccessAnimation.h = mLogo.h;
        mSuccessAnimation.startX = (windowProps.totalWindowWidth - mSuccessAnimation.w) / 2;
        mSuccessAnimation.startY = (windowProps.totalWindowHeight - mSuccessAnimation.h) / 2;
        mSuccessAnimation.currentX = mSuccessAnimation.startX;
        mSuccessAnimation.currentY = mSuccessAnimation.startY;
    }
}

void Screensaver::runSuccessScene()
{
    float dx = mSuccessAnimation.endX - mSuccessAnimation.currentX;
    float dy = mSuccessAnimation.endY - mSuccessAnimation.currentY;
    float length = sqrtf(dx * dx + dy * dy);
    float speed = 6.9f;
    if (length <= speed)
    {
        mSuccessAnimation.currentX = mSuccessAnimation.endX;
        mSuccessAnimation.currentY = mSuccessAnimation.endY;
    }
    else
    {
        mSuccessAnimation.currentX += (dx / length) * speed;
        mSuccessAnimation.currentY += (dy / length) * speed;
    }
}

bool Screensaver::isSuccess() const
{
    return mSuccess;
}

const SuccessAnimation &Screensaver::getSuccessAnimation() const
{
    return mSuccessAnimation;
}

bool Screensaver::isInactive() const
{
    return mInactive;
}

void Screensaver::resetTimer()
{
    mInactivityTimer = SDL_GetTicks();
    mInactive = false;
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

void Screensaver::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        resetTimer();
    }
}

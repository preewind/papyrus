#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

#include "Screensaver.h"
#include "logger.h"

Screensaver::Screensaver()
{
    mInactivityTimer = SDL_GetTicks();
    mLastFrameTimeMs = mInactivityTimer;
    mFrameTimeMs = mInactivityTimer;
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
    const uint32_t nowMs = SDL_GetTicks();
    const uint32_t deltaMs = nowMs - mLastFrameTimeMs;
    mLastFrameTimeMs = nowMs;
    mFrameTimeMs = nowMs;
    const float deltaSeconds = std::min(deltaMs / 1000.0f, 0.05f);

    if (mSuccess)
    {
        runSuccessScene(nowMs, deltaSeconds);
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
        mSuccessStartTimeMs = nowMs;
        mSuccessAnimation.active = true;
        mSuccessAnimation.endX = mLogo.x;
        mSuccessAnimation.endY = mLogo.y - mLogo.h / 5;
        mSuccessAnimation.w = mLogo.w;
        mSuccessAnimation.h = mLogo.h;
        mSuccessAnimation.startX = (windowProps.totalWindowWidth - mSuccessAnimation.w) / 2;
        mSuccessAnimation.startY = (windowProps.totalWindowHeight - mSuccessAnimation.h) / 2;
        mSuccessAnimation.currentX = mSuccessAnimation.startX;
        mSuccessAnimation.currentY = mSuccessAnimation.startY;

        // hit markers
        mMarkers.clear();
        size_t markerCount = 200;
        mMarkers.reserve(markerCount);
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());

        std::uniform_int_distribution<int> xdistrib(0, windowProps.totalWindowWidth);
        std::uniform_int_distribution<int> ydistrib(0, windowProps.totalWindowHeight);
        std::uniform_int_distribution<int> timedistrib(0, 600);
        for (size_t i = 0; i < markerCount; ++i)
        {
            HitMarker marker;
            marker.x = xdistrib(gen);
            marker.y = ydistrib(gen);
            marker.startOffset = timedistrib(gen);
            marker.duration = 0.5 * SDL_MS_PER_SECOND;
            mMarkers.push_back(marker);
        }
        // explosions
        mExplosions.clear();
        size_t explosionCount = 10;
        mExplosions.reserve(explosionCount);

        for (size_t i = 0; i < explosionCount; ++i)
        {
            Explosion explosion;
            explosion.x = xdistrib(gen);
            explosion.y = ydistrib(gen);
            explosion.w = 420.0f;
            explosion.h = 420.0f;
            explosion.startOffset = timedistrib(gen);
            explosion.duration = 0.5 * SDL_MS_PER_SECOND;
            mExplosions.push_back(explosion);
        }
        // wow guy
        mWow.w = 498*2;
        mWow.h = 280*2;
        mWow.x = (windowProps.totalWindowWidth-mWow.w)/2;
        mWow.y = (windowProps.totalWindowHeight - mWow.h)/2;
        mWow.startOffset = 0;
    }
}

void Screensaver::runSuccessScene(uint32_t nowMs, float deltaSeconds)
{
    float dx = mSuccessAnimation.endX - mSuccessAnimation.currentX;
    float dy = mSuccessAnimation.endY - mSuccessAnimation.currentY;
    float length = sqrtf(dx * dx + dy * dy);
    float movementStep = mSuccessAnimation.speedPixelsPerSecond * deltaSeconds;
    if (length <= movementStep)
    {
        mSuccessAnimation.currentX = mSuccessAnimation.endX;
        mSuccessAnimation.currentY = mSuccessAnimation.endY;

        if (mSuccessAnimation.active)
        {
            mSuccessAnimation.active = false;
            for (auto &marker : mMarkers)
            {
                marker.startTime = nowMs + marker.startOffset;
            }
            for (auto &explosion : mExplosions)
            {
                explosion.startTime = nowMs + explosion.startOffset;
            }
            mWow.startTime = nowMs;
        }

        bool allMarkersFinished = true;
        for (const auto &marker : mMarkers)
        {
            if (nowMs < marker.startTime + marker.duration)
            {
                allMarkersFinished = false;
                break;
            }
        }
        bool allExplosionsFinished = true;
        for(const auto& explosion: mExplosions){
            if(nowMs < explosion.startTime + explosion.duration){
                allExplosionsFinished = false;
                break;
            }
        }
        bool wowGuyFinished = true;
        if(nowMs < mWow.startTime + 1350){
            wowGuyFinished = false;
        }
        if (allMarkersFinished && allExplosionsFinished && wowGuyFinished)
        {
            mSuccess = false;
        }
    }
    else
    {
        mSuccessAnimation.currentX += (dx / length) * movementStep;
        mSuccessAnimation.currentY += (dy / length) * movementStep;
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

const std::vector<HitMarker> &Screensaver::getMarkers() const
{
    return mMarkers;
}

const std::vector<Explosion> &Screensaver::getExplosions() const
{
    return mExplosions;
}

const Wow &Screensaver::getWow() const
{
    return mWow;
}

uint32_t Screensaver::getFrameTimeMs() const
{
    return mFrameTimeMs;
}

uint32_t Screensaver::getSuccessElapsedMs() const
{
    if (!mSuccess)
    {
        return 0;
    }

    return mFrameTimeMs - mSuccessStartTimeMs;
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

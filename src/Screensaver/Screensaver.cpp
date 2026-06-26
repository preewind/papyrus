#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>

#include "Screensaver.h"
#include "Core/types.h"
#include "random.h"
#include "ScreensaverAssets.h"

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

DvdScreensaver &Screensaver::getDvdScreensaver()
{
    return mDvd;
}

const DvdScreensaver &Screensaver::getDvdScreensaverConst() const
{
    return mDvd;
}

void Screensaver::runScreensaver(const Window_Properties &windowProps)
{
    const uint32_t nowMs = SDL_GetTicks();
    const uint32_t deltaMs = nowMs - mLastFrameTimeMs;
    mLastFrameTimeMs = nowMs;
    mFrameTimeMs = nowMs;
    const float deltaSeconds = std::min(deltaMs / 1000.0f, 0.05f);
    mDvd.runScreensaver(windowProps, nowMs, deltaSeconds);
}

uint32_t Screensaver::getFrameTimeMs() const
{
    return mFrameTimeMs;
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

void Screensaver::setIntervalSeconds(uint64_t interval)
{
    if (interval > 0)
    {
        mInactivityInterval = interval;
    }
}

void Screensaver::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        resetTimer();
    }
}

#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <memory>

#include "DvdScreensaver.h"
#include "PongScreensaver.h"
#include "Screensaver.h"
#include "Core/types.h"

Screensaver::Screensaver()
{
    mInactivityTimer = SDL_GetTicks();
    mLastFrameTimeMs = mInactivityTimer;
    mFrameTimeMs = mInactivityTimer;

    mScenes.push_back(std::make_unique<DvdScreensaver>());
    mScenes.push_back(std::make_unique<PongScreensaver>());

    for (auto &scene : mScenes)
    {
        scene->reset();
    }
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

DvdScreensaver *Screensaver::getDvdScreensaver()
{
    for (auto &scene : mScenes)
    {
        auto *dvd = dynamic_cast<DvdScreensaver *>(scene.get());
        if (dvd != nullptr)
        {
            return dvd;
        }
    }
    return nullptr;
}

const DvdScreensaver *Screensaver::getDvdScreensaverConst() const
{
    for (const auto &scene : mScenes)
    {
        const auto *dvd = dynamic_cast<const DvdScreensaver *>(scene.get());
        if (dvd != nullptr)
        {
            return dvd;
        }
    }
    return nullptr;
}

const IScreensaverLogic *Screensaver::getActiveScene() const
{
    if (mScenes.empty())
    {
        return nullptr;
    }
    return mScenes[mActiveSceneIndex].get();
}

void Screensaver::runScreensaver(const Window_Properties &windowProps)
{
    if (mScenes.empty())
    {
        return;
    }

    const uint32_t nowMs = SDL_GetTicks();
    const uint32_t deltaMs = nowMs - mLastFrameTimeMs;
    mLastFrameTimeMs = nowMs;
    mFrameTimeMs = nowMs;
    const float deltaSeconds = std::min(deltaMs / 1000.0f, 0.05f);

    mScenes[mActiveSceneIndex]->update(windowProps, nowMs, deltaSeconds);
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
        if (event.key.key == SDLK_F5)
        {
            cycleScene();
        }
        else
        {
            resetTimer();
        }
    }
}

void Screensaver::cycleScene()
{
    if (mScenes.empty())
    {
        return;
    }

    mActiveSceneIndex = (mActiveSceneIndex + 1) % mScenes.size();
}

#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <SDL3/SDL_events.h>

#include "IScreensaverLogic.h"

class DvdScreensaver;
struct Window_Properties;

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    DvdScreensaver *getDvdScreensaver();
    const DvdScreensaver *getDvdScreensaverConst() const;
    const IScreensaverLogic *getActiveScene() const;
    bool isPlaying() const;
    void runScreensaver(const Window_Properties &windowProps);
    uint32_t getFrameTimeMs() const;
    bool isInactive() const;
    void resetTimer();
    void setIntervalSeconds(uint64_t interval);
    void handleKey(const SDL_Event &event);

private:
    void cycleScene();

    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 120;
    bool mInactive = false;
    uint32_t mLastFrameTimeMs = 0;
    uint32_t mFrameTimeMs = 0;
    std::vector<std::unique_ptr<IScreensaverLogic>> mScenes;
    size_t mActiveSceneIndex = 0;
};
#pragma once

#include <stdint.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_events.h>
#include <optional>

#include "DvdScreensaver.h"

struct Window_Properties;

enum class ScreensaverScene{
    DVD,
    Pong,
};

class Screensaver
{
public:
    Screensaver();
    void updateScreensaver();
    DvdScreensaver& getDvdScreensaver();
    const DvdScreensaver& getDvdScreensaverConst() const;
    void runScreensaver(const Window_Properties &windowProps);
    uint32_t getFrameTimeMs() const;
    bool isInactive() const;
    void resetTimer();
    void setIntervalSeconds(uint64_t interval);
    void handleKey(const SDL_Event &event);

private:

    uint64_t mInactivityTimer = 0;
    uint64_t mInactivityInterval = 60;
    bool mInactive = false;
    uint32_t mLastFrameTimeMs = 0;
    uint32_t mFrameTimeMs = 0;
    ScreensaverScene mScene = ScreensaverScene::DVD;
    DvdScreensaver mDvd;
};
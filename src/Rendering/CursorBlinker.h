#pragma once

#pragma once

#include <SDL3/SDL_stdinc.h>

class CursorBlinker
{
public:
    void update();

    void reset();

    bool visible() const;

private:
    Uint64 mLastBlink = 0;
    bool mVisible = true;
};
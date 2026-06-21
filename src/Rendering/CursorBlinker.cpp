#include <SDL3/SDL_timer.h>

#include "CursorBlinker.h"

void CursorBlinker::update()
{
    Uint64 now = SDL_GetTicks();

    if (now - mLastBlink > 500)
    {
        mVisible = !mVisible;
        mLastBlink = now;
    }
}

void CursorBlinker::reset()
{
    mVisible = true;
    mLastBlink = SDL_GetTicks();
}

bool CursorBlinker::visible() const
{
    return mVisible;
}

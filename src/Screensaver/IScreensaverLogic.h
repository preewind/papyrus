#pragma once

#include <stdint.h>

#include <SDL3/SDL_events.h>

struct Window_Properties;
class RenderContext;
class TextLayout;

class IScreensaverLogic
{
public:
    virtual ~IScreensaverLogic() = default;

    virtual void update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds) = 0;
    virtual void reset() = 0;
    virtual void render(RenderContext &renderContext, const TextLayout &textLayout, uint32_t frameTime) const = 0;
    virtual bool isPlaying() const = 0;
    virtual void handleKey(const SDL_Event &event)
    {
        (void)event;
    }
    virtual void setUserControlActive(bool active)
    {
        (void)active;
    }
};

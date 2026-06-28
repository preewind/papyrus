#pragma once

#include <stdint.h>

struct Window_Properties;
class RenderContext;

class IScreensaverLogic
{
public:
    virtual ~IScreensaverLogic() = default;

    virtual void update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds) = 0;
    virtual void reset() = 0;
    virtual void render(RenderContext &renderContext, uint32_t frameTime) const = 0;
    virtual bool isPlaying() const = 0;
};

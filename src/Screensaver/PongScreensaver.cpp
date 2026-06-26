#include "PongScreensaver.h"
#include "Rendering/RenderContext.h"

PongScreensaver::PongScreensaver()
{
    reset();
}

void PongScreensaver::update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds)
{
    (void)windowProps;
    (void)nowMs;
    (void)deltaSeconds;
}

void PongScreensaver::reset()
{
}

void PongScreensaver::render(RenderContext &renderContext, uint32_t frameTime) const
{
    (void)frameTime;
    renderContext.clear({255, 0, 255, 255});
}
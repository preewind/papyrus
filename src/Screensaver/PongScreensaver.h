#pragma once

#include "IScreensaverLogic.h"

struct Window_Properties;
class RenderContext;

class PongScreensaver : public IScreensaverLogic
{
public:
    PongScreensaver();
    void update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds) override;
    void reset() override;
    void render(RenderContext &renderContext, uint32_t frameTime) const override;
};

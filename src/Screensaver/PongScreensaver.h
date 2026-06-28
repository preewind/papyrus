#pragma once

#include <SDL3/SDL_events.h>

#include "IScreensaverLogic.h"
#include "types.h"

struct Window_Properties;
class RenderContext;

struct Paddle
{
    float x, y;
    float width, height;
    float velocity;
};

struct Ball
{
    float x, y;
    float width, height;
    float dx;
    float dy;
};

struct Score{
    float x,y;
    uint8_t score = 0;
};

class PongScreensaver : public IScreensaverLogic
{
public:
    PongScreensaver();
    void update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds) override;
    void reset() override;
    void render(RenderContext &renderContext, uint32_t frameTime) const override;
    void renderWin(RenderContext &renderContext) const;
    void handleKey(const SDL_Keycode &key);
    bool isPlaying() const override;

private:
    void initializePlayers(const Window_Properties &windowProps);
    Paddle mPlayer1;
    Paddle mPlayer2;
    Score mScoreP1;
    Score mScoreP2;
    bool mPlayer1Wins = false;
    bool mWin = false;
    Ball mBall;
    bool mPlaying = false;
    RectF mLine;
    bool mInitialized = false;
};

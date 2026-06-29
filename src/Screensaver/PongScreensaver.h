#pragma once

#include <SDL3/SDL_events.h>
#include <vector>

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

enum class PongMode{
    PVP,
    PVE,
    EVE
};

namespace PongModeUtils{
    inline std::string toString(PongMode mode) {
        switch (mode) {
            case PongMode::PVP:
                return "Player vs Player";
            case PongMode::PVE:
                return "Player vs CPU";
            case PongMode::EVE:
                return "CPU vs CPU";
            default:
                return "Unknown";
        }
    }
}

struct Level{
    float paddleSpeedMul;
    float ballSpeedMul;
    float paddleSizeMul;
};

struct MenuLayout {
    float levelTextX;
    float levelTextY;
    float gameModeTextX;
    float gameModeTextY;

};

struct GameOverMenu {

};

class PongScreensaver : public IScreensaverLogic
{
public:
    PongScreensaver();
    void update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds) override;
    void reset() override;
    void render(RenderContext &renderContext, uint32_t frameTime) const override;
    void renderWin(RenderContext &renderContext) const;
    void renderMenu(RenderContext &renderContext) const;
    void handleKey(const SDL_Event &event);
    bool isPlaying() const override;
    

private:
    void initializePlayers(const Window_Properties &windowProps);
    void updatePlayerMovement(const Window_Properties &windowProps);
    void handlePaddleCollision(const Paddle &paddle, bool isPlayer1);
    void initializeLayout(const Window_Properties &windowProps);
    void handleMenuInput(const SDL_Event &event);
    void handleGameplayInput(const SDL_Event &event);
    void handleLeft();
    void handleRight();
    void handleUp();
    void handleDown();
    
    Paddle mPlayer1;
    Paddle mPlayer2;
    Score mScoreP1;
    Score mScoreP2;
    bool mPlayer1Wins = false;
    bool mWin = false;
    Ball mBall;
    bool mPlaying = false;
    RectF mLine;
    bool mMenuInitialized = false;
    bool mInitialized = false;
    bool mMenuActive = true;
    uint8_t mMenuIndex = 0;
    
    // levels
    PongMode mCurrentMode = PongMode::PVP;
    std::vector<Level> mLevels;
    uint8_t mCurrentLvl = 0;
    MenuLayout mMenu;

    bool mKeyW = false;
    bool mKeyS = false;
    bool mKeyUp = false;
    bool mKeyDown = false;
};

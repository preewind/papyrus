#pragma once

#include <SDL3/SDL_events.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "IScreensaverLogic.h"
#include "types.h"

struct Window_Properties;
class RenderContext;
class TextLayout;

struct Paddle
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float velocity = 0.0f;
};

struct Ball
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
};

struct Score
{
    float x = 0.0f;
    float y = 0.0f;
    uint8_t score = 0;
};

enum class PongMode
{
    PVP,
    PVE,
    EVE
};

namespace PongModeUtils
{
    inline std::string toString(PongMode mode)
    {
        switch (mode)
        {
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

struct Level
{
    float paddleSpeedMul = 1.0f;
    float ballSpeedMul = 1.0f;
    float paddleSizeMul = 1.0f;
};

enum class PongSceneState : uint8_t
{
    Demo,
    Menu,
    Match,
    Pause,
    Win
};

enum class PongMenuRow : uint8_t
{
    Level,
    GameMode
};

enum class PongPauseAction : uint8_t
{
    Resume,
    RestartMatch,
    MainMenu
};

enum class PongWinAction : uint8_t
{
    RestartMatch,
    NextLevel,
    MainMenu
};

struct PongSettings
{
    PongMode mode = PongMode::PVP;
    size_t levelIndex = 0;
};

struct PongMatchState
{
    Paddle player1;
    Paddle player2;
    Score scoreP1;
    Score scoreP2;
    Ball ball;
    RectF centerLine;
    bool player1Won = false;
};

class PongScreensaver : public IScreensaverLogic
{
public:
    PongScreensaver();

    void update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds) override;
    void reset() override;
    void render(RenderContext &renderContext, const TextLayout &textLayout, uint32_t frameTime) const override;
    void handleKey(const SDL_Event &event) override;
    bool isPlaying() const override;
    void setUserControlActive(bool active) override;

private:
    void enterDemo();
    void enterMenu();
    void startMatch();
    void pauseMatch();
    void resumeMatch();
    void finishMatch(bool player1Won);
    void restartMatchWithCurrentSettings();
    void startNextLevel();
    void returnToMenu();

    void resetMatchState();
    void clearInputState();
    void ensureArenaInitialized(const Window_Properties &windowProps);
    void initializeArena(const Window_Properties &windowProps);
    void resetRound(const Window_Properties &windowProps, bool serveTowardsPlayer2);

    void updateMatchControllers(const Window_Properties &windowProps);
    void updatePaddle(Paddle &paddle, bool moveUp, bool moveDown, const Window_Properties &windowProps);
    void updateCpuPaddle(Paddle &paddle, const Window_Properties &windowProps) const;
    void clampPaddle(Paddle &paddle, const Window_Properties &windowProps) const;
    void updateBall(const Window_Properties &windowProps, bool demoMode);
    void handlePointScored(const Window_Properties &windowProps, bool player1Scored, bool demoMode);
    void handlePaddleCollision(Paddle &paddle, bool isPlayer1);
    bool player1UsesCpu() const;
    bool player2UsesCpu() const;

    void handleDemoInput(const SDL_Event &event);
    void handleMenuInput(const SDL_Event &event);
    void handleMatchInput(const SDL_Event &event);
    void handlePauseInput(const SDL_Event &event);
    void handleWinInput(const SDL_Event &event);
    void handleLeft();
    void handleRight();
    void handleUp();
    void handleDown();
    void cyclePauseSelection(int delta);
    void cycleWinSelection(int delta);
    void executePauseAction();
    void executeWinAction();

    void renderArena(RenderContext &renderContext) const;
    void renderDemo(RenderContext &renderContext, const TextLayout &textLayout, uint32_t frameTime) const;
    void renderMenu(RenderContext &renderContext, const TextLayout &textLayout) const;
    void renderPause(RenderContext &renderContext, const TextLayout &textLayout) const;
    void renderWin(RenderContext &renderContext, const TextLayout &textLayout) const;

    PongSceneState mState = PongSceneState::Demo;
    PongMenuRow mMenuSelection = PongMenuRow::Level;
    PongPauseAction mPauseSelection = PongPauseAction::Resume;
    PongWinAction mWinSelection = PongWinAction::RestartMatch;
    PongSettings mSettings;
    PongMatchState mMatch;
    std::vector<Level> mLevels;
    bool mArenaInitialized = false;
    bool mServeTowardsPlayer2 = true;
    bool mInteractiveFocus = false;

    bool mKeyW = false;
    bool mKeyS = false;
    bool mKeyUp = false;
    bool mKeyDown = false;
};

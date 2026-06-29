#include "PongScreensaver.h"

#include <SDL3/SDL_keycode.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "Rendering/RenderContext.h"
#include "TextLayout.h"

namespace
{
const RenderColor kWhite{255, 255, 255, 255};
const RenderColor kBackground{0, 0, 0, 255};
constexpr float kPaddleWidth = 20.0f;
constexpr float kBallSize = 20.0f;
constexpr float kArenaPadding = 20.0f;
constexpr float kCenterLineWidth = 5.0f;
constexpr float kBaseBallSpeed = 5.0f;
constexpr float kBasePaddleHeight = 120.0f;
constexpr float kBasePaddleVelocity = 10.0f;
constexpr float kCpuDeadZone = 12.0f;
constexpr uint8_t kScoreToWin = 3;
constexpr uint32_t kHintFlashIntervalMs = 450;
constexpr float kPi = 3.14159265f;

struct LabeledValueRow
{
    std::string label;
    std::string value;
};

bool isSingleKeyPress(const SDL_Event &event)
{
    return event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat;
}

bool isEnterKey(SDL_Keycode key)
{
    return key == SDLK_RETURN || key == SDLK_KP_ENTER;
}

int centeredTextX(const Window_Properties &windowProps, const TextLayout &textLayout, const std::string &text)
{
    const int availableWidth = static_cast<int>(windowProps.totalWindowWidth);
    const int textWidth = static_cast<int>(textLayout.width(text));
    return std::max(0, (availableWidth - textWidth) / 2);
}

void drawCenteredText(RenderContext &renderContext, const TextLayout &textLayout,
                      const Window_Properties &windowProps, const std::string &text, int y)
{
    renderContext.drawText(text, centeredTextX(windowProps, textLayout, text), y, kWhite);
}

void drawCenteredOptions(RenderContext &renderContext, const TextLayout &textLayout,
                         const Window_Properties &windowProps, int startY, int rowSpacing,
                         const std::vector<std::string> &options)
{
    for (size_t index = 0; index < options.size(); ++index)
    {
        drawCenteredText(renderContext, textLayout, windowProps, options[index], startY + static_cast<int>(index) * rowSpacing);
    }
}

void drawLabeledRows(RenderContext &renderContext, const TextLayout &textLayout,
                     const Window_Properties &windowProps, int startY, int rowSpacing,
                     const std::vector<LabeledValueRow> &rows)
{
    uint32_t maxLabelWidth = 0;
    uint32_t blockWidth = 0;
    const uint32_t separatorWidth = textLayout.width(":  ");

    for (const LabeledValueRow &row : rows)
    {
        maxLabelWidth = std::max(maxLabelWidth, textLayout.width(row.label));
    }

    for (const LabeledValueRow &row : rows)
    {
        const uint32_t rowWidth = maxLabelWidth + separatorWidth + textLayout.width(row.value);
        blockWidth = std::max(blockWidth, rowWidth);
    }

    const int blockX = std::max(0, (static_cast<int>(windowProps.totalWindowWidth) - static_cast<int>(blockWidth)) / 2);
    const int colonX = blockX + static_cast<int>(maxLabelWidth);
    const int valueX = colonX + static_cast<int>(separatorWidth);

    for (size_t index = 0; index < rows.size(); ++index)
    {
        const LabeledValueRow &row = rows[index];
        const int rowY = startY + static_cast<int>(index) * rowSpacing;
        const int labelX = blockX + static_cast<int>(maxLabelWidth - textLayout.width(row.label));
        renderContext.drawText(row.label, labelX, rowY, kWhite);
        renderContext.drawText(":", colonX, rowY, kWhite);
        renderContext.drawText(row.value, valueX, rowY, kWhite);
    }
}

std::string wrapSelection(const std::string &text, bool selected)
{
    if (!selected)
    {
        return text;
    }
    return "<" + text + ">";
}
}

PongScreensaver::PongScreensaver()
    : mLevels{{1.0f, 1.0f, 1.0f}, {1.0f, 1.1f, 1.0f}, {1.15f, 1.2f, 0.85f}}
{
    reset();
}

void PongScreensaver::update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds)
{
    (void)nowMs;
    (void)deltaSeconds;

    if (mState == PongSceneState::Menu)
    {
        return;
    }

    ensureArenaInitialized(windowProps);

    switch (mState)
    {
    case PongSceneState::Demo:
    case PongSceneState::Match:
        updateMatchControllers(windowProps);
        updateBall(windowProps, mState == PongSceneState::Demo);
        break;
    case PongSceneState::Pause:
    case PongSceneState::Win:
    case PongSceneState::Menu:
        break;
    }
}

void PongScreensaver::reset()
{
    mSettings = {};
    mMenuSelection = PongMenuRow::Level;
    mPauseSelection = PongPauseAction::Resume;
    mWinSelection = PongWinAction::RestartMatch;
    mServeTowardsPlayer2 = true;
    mInteractiveFocus = false;
    clearInputState();
    enterDemo();
}

void PongScreensaver::render(RenderContext &renderContext, const TextLayout &textLayout, uint32_t frameTime) const
{
    renderContext.clear(kBackground);

    switch (mState)
    {
    case PongSceneState::Demo:
        renderDemo(renderContext, textLayout, frameTime);
        break;
    case PongSceneState::Menu:
        renderMenu(renderContext, textLayout);
        break;
    case PongSceneState::Match:
        renderArena(renderContext);
        break;
    case PongSceneState::Pause:
        renderPause(renderContext, textLayout);
        break;
    case PongSceneState::Win:
        renderWin(renderContext, textLayout);
        break;
    }
}

void PongScreensaver::handleKey(const SDL_Event &event)
{
    switch (mState)
    {
    case PongSceneState::Demo:
        handleDemoInput(event);
        break;
    case PongSceneState::Menu:
        handleMenuInput(event);
        break;
    case PongSceneState::Match:
        handleMatchInput(event);
        break;
    case PongSceneState::Pause:
        handlePauseInput(event);
        break;
    case PongSceneState::Win:
        handleWinInput(event);
        break;
    }
}

bool PongScreensaver::isPlaying() const
{
    return mInteractiveFocus;
}

void PongScreensaver::setUserControlActive(bool active)
{
    mInteractiveFocus = active;
}

void PongScreensaver::enterDemo()
{
    mState = PongSceneState::Demo;
    resetMatchState();
}

void PongScreensaver::enterMenu()
{
    mState = PongSceneState::Menu;
    mMenuSelection = PongMenuRow::Level;
    clearInputState();
}

void PongScreensaver::startMatch()
{
    restartMatchWithCurrentSettings();
}

void PongScreensaver::pauseMatch()
{
    mState = PongSceneState::Pause;
    mPauseSelection = PongPauseAction::Resume;
    clearInputState();
}

void PongScreensaver::resumeMatch()
{
    mState = PongSceneState::Match;
    clearInputState();
}

void PongScreensaver::finishMatch(bool player1Won)
{
    mMatch.player1Won = player1Won;
    mState = PongSceneState::Win;
    mWinSelection = PongWinAction::RestartMatch;
    clearInputState();
}

void PongScreensaver::restartMatchWithCurrentSettings()
{
    mState = PongSceneState::Match;
    resetMatchState();
}

void PongScreensaver::startNextLevel()
{
    if (mSettings.levelIndex + 1 < mLevels.size())
    {
        ++mSettings.levelIndex;
    }
    restartMatchWithCurrentSettings();
}

void PongScreensaver::returnToMenu()
{
    mState = PongSceneState::Menu;
    mMenuSelection = PongMenuRow::Level;
    mPauseSelection = PongPauseAction::Resume;
    mWinSelection = PongWinAction::RestartMatch;
    resetMatchState();
}

void PongScreensaver::resetMatchState()
{
    mMatch = {};
    mArenaInitialized = false;
    mServeTowardsPlayer2 = true;
    clearInputState();
}

void PongScreensaver::clearInputState()
{
    mKeyW = false;
    mKeyS = false;
    mKeyUp = false;
    mKeyDown = false;
}

void PongScreensaver::ensureArenaInitialized(const Window_Properties &windowProps)
{
    if (!mArenaInitialized)
    {
        initializeArena(windowProps);
    }
}

void PongScreensaver::initializeArena(const Window_Properties &windowProps)
{
    const Level &level = mLevels[mSettings.levelIndex];
    const float paddleHeight = kBasePaddleHeight * level.paddleSizeMul;
    const float paddleVelocity = kBasePaddleVelocity * level.paddleSpeedMul;
    const float centeredPaddleY = (static_cast<float>(windowProps.totalWindowHeight) - paddleHeight) / 2.0f;

    mMatch.player1 = {kArenaPadding, centeredPaddleY, kPaddleWidth, paddleHeight, paddleVelocity};
    mMatch.player2 = {static_cast<float>(windowProps.totalWindowWidth) - kPaddleWidth - kArenaPadding, centeredPaddleY, kPaddleWidth, paddleHeight, paddleVelocity};
    mMatch.centerLine = {
        (static_cast<float>(windowProps.totalWindowWidth) - kCenterLineWidth) / 2.0f,
        kArenaPadding,
        kCenterLineWidth,
        static_cast<float>(windowProps.totalWindowHeight) - kArenaPadding,
    };

    mMatch.scoreP1.x = static_cast<float>(windowProps.totalWindowWidth) / 4.0f;
    mMatch.scoreP1.y = 50.0f;
    mMatch.scoreP2.x = static_cast<float>(windowProps.totalWindowWidth) - static_cast<float>(windowProps.totalWindowWidth) / 4.0f;
    mMatch.scoreP2.y = 50.0f;

    resetRound(windowProps, mServeTowardsPlayer2);
    mArenaInitialized = true;
}

void PongScreensaver::resetRound(const Window_Properties &windowProps, bool serveTowardsPlayer2)
{
    const Level &level = mLevels[mSettings.levelIndex];
    const float centeredPaddleY = (static_cast<float>(windowProps.totalWindowHeight) - mMatch.player1.height) / 2.0f;
    mMatch.player1.y = centeredPaddleY;
    mMatch.player2.y = centeredPaddleY;
    mMatch.ball = {
        (static_cast<float>(windowProps.totalWindowWidth) - kBallSize) / 2.0f,
        (static_cast<float>(windowProps.totalWindowHeight) - kBallSize) / 2.0f,
        kBallSize,
        kBallSize,
        serveTowardsPlayer2 ? kBaseBallSpeed * level.ballSpeedMul : -kBaseBallSpeed * level.ballSpeedMul,
        kBaseBallSpeed * 0.35f * level.ballSpeedMul,
    };
}

void PongScreensaver::updateMatchControllers(const Window_Properties &windowProps)
{
    if (player1UsesCpu())
    {
        updateCpuPaddle(mMatch.player1, windowProps);
    }
    else
    {
        updatePaddle(mMatch.player1, mKeyW, mKeyS, windowProps);
    }

    if (player2UsesCpu())
    {
        updateCpuPaddle(mMatch.player2, windowProps);
    }
    else
    {
        updatePaddle(mMatch.player2, mKeyUp, mKeyDown, windowProps);
    }
}

void PongScreensaver::updatePaddle(Paddle &paddle, bool moveUp, bool moveDown, const Window_Properties &windowProps)
{
    if (moveUp)
    {
        paddle.y -= paddle.velocity;
    }
    if (moveDown)
    {
        paddle.y += paddle.velocity;
    }
    clampPaddle(paddle, windowProps);
}

void PongScreensaver::updateCpuPaddle(Paddle &paddle, const Window_Properties &windowProps) const
{
    const float ballCenter = mMatch.ball.y + mMatch.ball.height / 2.0f;
    const float paddleCenter = paddle.y + paddle.height / 2.0f;
    if (ballCenter < paddleCenter - kCpuDeadZone)
    {
        paddle.y -= paddle.velocity;
    }
    else if (ballCenter > paddleCenter + kCpuDeadZone)
    {
        paddle.y += paddle.velocity;
    }
    clampPaddle(paddle, windowProps);
}

void PongScreensaver::clampPaddle(Paddle &paddle, const Window_Properties &windowProps) const
{
    const float maxY = static_cast<float>(windowProps.totalWindowHeight) - paddle.height;
    paddle.y = std::clamp(paddle.y, 0.0f, maxY);
}

void PongScreensaver::updateBall(const Window_Properties &windowProps, bool demoMode)
{
    mMatch.ball.x += mMatch.ball.dx;
    mMatch.ball.y += mMatch.ball.dy;

    const float maxY = static_cast<float>(windowProps.totalWindowHeight) - mMatch.ball.height;
    if (mMatch.ball.y >= maxY)
    {
        mMatch.ball.y = maxY;
        mMatch.ball.dy *= -1.0f;
    }
    else if (mMatch.ball.y <= 0.0f)
    {
        mMatch.ball.y = 0.0f;
        mMatch.ball.dy *= -1.0f;
    }

    const bool overlapsPlayer1Y = mMatch.ball.y + mMatch.ball.height >= mMatch.player1.y &&
                                  mMatch.ball.y <= mMatch.player1.y + mMatch.player1.height;
    const bool overlapsPlayer2Y = mMatch.ball.y + mMatch.ball.height >= mMatch.player2.y &&
                                  mMatch.ball.y <= mMatch.player2.y + mMatch.player2.height;

    if (mMatch.ball.dx < 0.0f &&
        mMatch.ball.x <= mMatch.player1.x + mMatch.player1.width &&
        mMatch.ball.x + mMatch.ball.width >= mMatch.player1.x &&
        overlapsPlayer1Y)
    {
        handlePaddleCollision(mMatch.player1, true);
    }
    else if (mMatch.ball.dx > 0.0f &&
             mMatch.ball.x + mMatch.ball.width >= mMatch.player2.x &&
             mMatch.ball.x <= mMatch.player2.x + mMatch.player2.width &&
             overlapsPlayer2Y)
    {
        handlePaddleCollision(mMatch.player2, false);
    }

    if (mMatch.ball.x + mMatch.ball.width >= static_cast<float>(windowProps.totalWindowWidth))
    {
        handlePointScored(windowProps, true, demoMode);
    }
    else if (mMatch.ball.x <= 0.0f)
    {
        handlePointScored(windowProps, false, demoMode);
    }
}

void PongScreensaver::handlePointScored(const Window_Properties &windowProps, bool player1Scored, bool demoMode)
{
    if (player1Scored)
    {
        ++mMatch.scoreP1.score;
    }
    else
    {
        ++mMatch.scoreP2.score;
    }

    if (!demoMode && (mMatch.scoreP1.score >= kScoreToWin || mMatch.scoreP2.score >= kScoreToWin))
    {
        finishMatch(player1Scored);
        return;
    }

    if (demoMode && (mMatch.scoreP1.score >= kScoreToWin || mMatch.scoreP2.score >= kScoreToWin))
    {
        mMatch.scoreP1.score = 0;
        mMatch.scoreP2.score = 0;
    }

    mServeTowardsPlayer2 = player1Scored;
    resetRound(windowProps, mServeTowardsPlayer2);
}

void PongScreensaver::handlePaddleCollision(Paddle &paddle, bool isPlayer1)
{
    const float ballCenterY = mMatch.ball.y + mMatch.ball.height / 2.0f;
    const float paddleCenterY = paddle.y + paddle.height / 2.0f;
    float relativeIntersectY = (paddleCenterY - ballCenterY) / (paddle.height / 2.0f);
    relativeIntersectY = std::clamp(relativeIntersectY, -1.0f, 1.0f);

    const float maxAngleRadians = 75.0f * kPi / 180.0f;
    const float exitAngle = -relativeIntersectY * maxAngleRadians;
    float ballSpeed = std::sqrt(mMatch.ball.dx * mMatch.ball.dx + mMatch.ball.dy * mMatch.ball.dy);
    ballSpeed *= 1.05f;

    if (isPlayer1)
    {
        mMatch.ball.x = paddle.x + paddle.width;
        mMatch.ball.dx = ballSpeed * std::cos(exitAngle);
    }
    else
    {
        mMatch.ball.x = paddle.x - mMatch.ball.width;
        mMatch.ball.dx = -ballSpeed * std::cos(exitAngle);
    }
    mMatch.ball.dy = ballSpeed * std::sin(exitAngle);
}

bool PongScreensaver::player1UsesCpu() const
{
    return mState == PongSceneState::Demo || mSettings.mode == PongMode::EVE;
}

bool PongScreensaver::player2UsesCpu() const
{
    return mState == PongSceneState::Demo || mSettings.mode == PongMode::PVE || mSettings.mode == PongMode::EVE;
}

void PongScreensaver::handleDemoInput(const SDL_Event &event)
{
    if (!isSingleKeyPress(event))
    {
        return;
    }

    if (isEnterKey(event.key.key) && (event.key.mod & SDL_KMOD_CTRL))
    {
        mInteractiveFocus = true;
        enterMenu();
    }
}

void PongScreensaver::handleMenuInput(const SDL_Event &event)
{
    if (!isSingleKeyPress(event))
    {
        return;
    }

    switch (event.key.key)
    {
    case SDLK_RIGHT:
        handleRight();
        break;
    case SDLK_LEFT:
        handleLeft();
        break;
    case SDLK_UP:
        handleUp();
        break;
    case SDLK_DOWN:
        handleDown();
        break;
    case SDLK_ESCAPE:
        enterDemo();
        break;
    default:
        if (isEnterKey(event.key.key))
        {
            startMatch();
        }
        break;
    }
}

void PongScreensaver::handleMatchInput(const SDL_Event &event)
{
    if (event.type != SDL_EVENT_KEY_DOWN && event.type != SDL_EVENT_KEY_UP)
    {
        return;
    }

    if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.key == SDLK_ESCAPE)
    {
        pauseMatch();
        return;
    }

    const bool isKeyDown = event.type == SDL_EVENT_KEY_DOWN;
    switch (event.key.key)
    {
    case SDLK_UP:
        mKeyUp = isKeyDown;
        break;
    case SDLK_DOWN:
        mKeyDown = isKeyDown;
        break;
    case SDLK_W:
        mKeyW = isKeyDown;
        break;
    case SDLK_S:
        mKeyS = isKeyDown;
        break;
    default:
        break;
    }
}

void PongScreensaver::handlePauseInput(const SDL_Event &event)
{
    if (!isSingleKeyPress(event))
    {
        return;
    }

    switch (event.key.key)
    {
    case SDLK_ESCAPE:
        resumeMatch();
        break;
    case SDLK_UP:
        cyclePauseSelection(-1);
        break;
    case SDLK_DOWN:
        cyclePauseSelection(1);
        break;
    default:
        if (isEnterKey(event.key.key))
        {
            executePauseAction();
        }
        break;
    }
}

void PongScreensaver::handleWinInput(const SDL_Event &event)
{
    if (!isSingleKeyPress(event))
    {
        return;
    }

    switch (event.key.key)
    {
    case SDLK_UP:
        cycleWinSelection(-1);
        break;
    case SDLK_DOWN:
        cycleWinSelection(1);
        break;
    case SDLK_ESCAPE:
        returnToMenu();
        break;
    default:
        if (isEnterKey(event.key.key))
        {
            executeWinAction();
        }
        break;
    }
}

void PongScreensaver::handleLeft()
{
    if (mMenuSelection == PongMenuRow::Level)
    {
        mSettings.levelIndex = (mSettings.levelIndex + mLevels.size() - 1) % mLevels.size();
        return;
    }

    const int currentMode = static_cast<int>(mSettings.mode);
    mSettings.mode = static_cast<PongMode>((currentMode + 2) % 3);
}

void PongScreensaver::handleRight()
{
    if (mMenuSelection == PongMenuRow::Level)
    {
        mSettings.levelIndex = (mSettings.levelIndex + 1) % mLevels.size();
        return;
    }

    const int currentMode = static_cast<int>(mSettings.mode);
    mSettings.mode = static_cast<PongMode>((currentMode + 1) % 3);
}

void PongScreensaver::handleUp()
{
    mMenuSelection = mMenuSelection == PongMenuRow::Level ? PongMenuRow::GameMode : PongMenuRow::Level;
}

void PongScreensaver::handleDown()
{
    mMenuSelection = mMenuSelection == PongMenuRow::Level ? PongMenuRow::GameMode : PongMenuRow::Level;
}

void PongScreensaver::cyclePauseSelection(int delta)
{
    const int count = 3;
    const int current = static_cast<int>(mPauseSelection);
    mPauseSelection = static_cast<PongPauseAction>((current + delta + count) % count);
}

void PongScreensaver::cycleWinSelection(int delta)
{
    const int count = 3;
    const int current = static_cast<int>(mWinSelection);
    mWinSelection = static_cast<PongWinAction>((current + delta + count) % count);
}

void PongScreensaver::executePauseAction()
{
    switch (mPauseSelection)
    {
    case PongPauseAction::Resume:
        resumeMatch();
        break;
    case PongPauseAction::RestartMatch:
        restartMatchWithCurrentSettings();
        break;
    case PongPauseAction::MainMenu:
        returnToMenu();
        break;
    }
}

void PongScreensaver::executeWinAction()
{
    switch (mWinSelection)
    {
    case PongWinAction::RestartMatch:
        restartMatchWithCurrentSettings();
        break;
    case PongWinAction::NextLevel:
        startNextLevel();
        break;
    case PongWinAction::MainMenu:
        returnToMenu();
        break;
    }
}

void PongScreensaver::renderArena(RenderContext &renderContext) const
{
    renderContext.drawRect(mMatch.player1.x, mMatch.player1.y, mMatch.player1.width, mMatch.player1.height, kWhite);
    renderContext.drawRect(mMatch.player2.x, mMatch.player2.y, mMatch.player2.width, mMatch.player2.height, kWhite);
    renderContext.drawDottedLine(mMatch.centerLine.x, mMatch.centerLine.y, mMatch.centerLine.h, mMatch.centerLine.w, 20.0f, 50.0f, kWhite);
    renderContext.drawRect(mMatch.ball.x, mMatch.ball.y, mMatch.ball.width, mMatch.ball.height, kWhite);
    renderContext.drawText(std::to_string(mMatch.scoreP1.score), static_cast<int>(mMatch.scoreP1.x), static_cast<int>(mMatch.scoreP1.y), kWhite);
    renderContext.drawText(std::to_string(mMatch.scoreP2.score), static_cast<int>(mMatch.scoreP2.x), static_cast<int>(mMatch.scoreP2.y), kWhite);
}

void PongScreensaver::renderDemo(RenderContext &renderContext, const TextLayout &textLayout, uint32_t frameTime) const
{
    renderArena(renderContext);

    const Window_Properties &windowProps = renderContext.getWindowProperties();
    const int topY = std::max(16, static_cast<int>(windowProps.lineHeight));
    const int bottomY = std::max(16, static_cast<int>(windowProps.totalWindowHeight) - static_cast<int>(windowProps.lineHeight) * 3);

    drawCenteredText(renderContext, textLayout, windowProps, "Demo mode", topY);
    if (((frameTime / kHintFlashIntervalMs) % 2u) == 0u)
    {
        drawCenteredText(renderContext, textLayout, windowProps, "Press Ctrl+Return to play", bottomY);
    }
}

void PongScreensaver::renderMenu(RenderContext &renderContext, const TextLayout &textLayout) const
{
    const Window_Properties &windowProps = renderContext.getWindowProperties();
    const int lineHeight = std::max(20, static_cast<int>(windowProps.lineHeight));
    const int rowSpacing = lineHeight + 10;
    const int titleY = std::max(30, static_cast<int>(windowProps.totalWindowHeight) / 4 - lineHeight);
    const int rowsY = static_cast<int>(windowProps.totalWindowHeight) / 2 - rowSpacing;
    const int footerY = rowsY + rowSpacing * 3;

    const std::vector<LabeledValueRow> rows{
        {"Level", wrapSelection(std::to_string(mSettings.levelIndex + 1), mMenuSelection == PongMenuRow::Level)},
        {"Gamemode", wrapSelection(PongModeUtils::toString(mSettings.mode), mMenuSelection == PongMenuRow::GameMode)},
    };

    drawCenteredText(renderContext, textLayout, windowProps, "Pong", titleY);
    drawLabeledRows(renderContext, textLayout, windowProps, rowsY, rowSpacing, rows);
    drawCenteredText(renderContext, textLayout, windowProps, "Press Enter to start", footerY);
    drawCenteredText(renderContext, textLayout, windowProps, "Press Escape to return to demo", footerY + rowSpacing);
}

void PongScreensaver::renderPause(RenderContext &renderContext, const TextLayout &textLayout) const
{
    const Window_Properties &windowProps = renderContext.getWindowProperties();
    const int lineHeight = std::max(20, static_cast<int>(windowProps.lineHeight));
    const int rowSpacing = lineHeight + 10;
    const int titleY = std::max(30, static_cast<int>(windowProps.totalWindowHeight) / 4 - lineHeight);
    const int infoY = titleY + rowSpacing * 2;
    const int optionsY = infoY + rowSpacing * 2;

    drawCenteredText(renderContext, textLayout, windowProps, "Pause", titleY);
    drawCenteredText(renderContext, textLayout, windowProps,
                     "Score: " + std::to_string(mMatch.scoreP1.score) + " - " + std::to_string(mMatch.scoreP2.score), infoY);
    drawCenteredOptions(renderContext, textLayout, windowProps, optionsY, rowSpacing,
                        {
                            wrapSelection("Resume", mPauseSelection == PongPauseAction::Resume),
                            wrapSelection("Restart Match", mPauseSelection == PongPauseAction::RestartMatch),
                            wrapSelection("Main Menu", mPauseSelection == PongPauseAction::MainMenu),
                        });
    drawCenteredText(renderContext, textLayout, windowProps, "Press Escape to continue", optionsY + rowSpacing * 4);
}

void PongScreensaver::renderWin(RenderContext &renderContext, const TextLayout &textLayout) const
{
    const Window_Properties &windowProps = renderContext.getWindowProperties();
    const int lineHeight = std::max(20, static_cast<int>(windowProps.lineHeight));
    const int rowSpacing = lineHeight + 10;
    const int titleY = std::max(30, static_cast<int>(windowProps.totalWindowHeight) / 4 - lineHeight);
    const int infoY = titleY + rowSpacing * 2;
    const int optionsY = infoY + rowSpacing * 2;

    drawCenteredText(renderContext, textLayout, windowProps, mMatch.player1Won ? "Player 1 wins" : "Player 2 wins", titleY);
    drawCenteredText(renderContext, textLayout, windowProps,
                     "Score: " + std::to_string(mMatch.scoreP1.score) + " - " + std::to_string(mMatch.scoreP2.score), infoY);
    drawCenteredOptions(renderContext, textLayout, windowProps, optionsY, rowSpacing,
                        {
                            wrapSelection("Restart Match", mWinSelection == PongWinAction::RestartMatch),
                            wrapSelection("Next Level", mWinSelection == PongWinAction::NextLevel),
                            wrapSelection("Main Menu", mWinSelection == PongWinAction::MainMenu),
                        });
}

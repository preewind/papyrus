#include "PongScreensaver.h"
#include "Rendering/RenderContext.h"
#include <cmath>

PongScreensaver::PongScreensaver()
{
    reset();
    mPlaying = true; // TODO: activate only when player actively wants to play and not when seeing the demo game
}

void PongScreensaver::update(const Window_Properties &windowProps, uint32_t nowMs, float deltaSeconds)
{
    if (!mInitialized)
    {
        initializePlayers(windowProps);
        mInitialized = true;
    }
    (void)nowMs;
    (void)deltaSeconds;

    updatePlayerMovement(windowProps);

    if (mScoreP1.score >= 10)
    {
        mPlayer1Wins = true;
        mWin = true;
        return;
    }
    else if (mScoreP2.score >= 10)
    {
        mPlayer1Wins = false;
        mWin = true;
        return;
    }

    mBall.x += mBall.dx;
    mBall.y += mBall.dy;

    // wall collisions / win conditions
    float maxX = windowProps.totalWindowWidth - mBall.width;
    if (mBall.x >= maxX)
    {
        reset();
        mScoreP1.score++;
    }
    else if (mBall.x <= 0)
    {
        reset();
        mScoreP2.score++;
    }
    float maxY = windowProps.totalWindowHeight - mBall.height;
    if (mBall.y >= maxY)
    {
        mBall.y = maxY;
        mBall.dy *= -1;
    }
    else if (mBall.y <= 0)
    {
        mBall.y = 0;
        mBall.dy *= -1;
    }

    // paddle collisions
    if (mBall.x  >= mPlayer1.x && mBall.x <= mPlayer1.x + mPlayer1.width && mBall.y >= mPlayer1.y && mBall.y <= mPlayer1.y + mPlayer1.height)
    {
        handlePaddleCollision(mPlayer1, true);
    }

    if (mBall.x + mBall.width >= mPlayer2.x && mBall.x + mBall.width <= mPlayer2.x + mPlayer2.width && mBall.y >= mPlayer2.y && mBall.y <= mPlayer2.y + mPlayer2.height)
    {
        handlePaddleCollision(mPlayer2, false);
    }
}

void PongScreensaver::reset()
{
    mInitialized = false;
}

void PongScreensaver::render(RenderContext &renderContext, uint32_t frameTime) const
{

    if (mWin)
    {
        renderWin(renderContext);
        return;
    }

    RenderColor color{255, 255, 255, 255};
    (void)frameTime;
    renderContext.clear({0, 0, 0, 255});

    renderContext.drawRect(mPlayer1.x, mPlayer1.y, mPlayer1.width, mPlayer1.height, color);
    renderContext.drawRect(mPlayer2.x, mPlayer2.y, mPlayer2.width, mPlayer2.height, color);

    renderContext.drawDottedLine(mLine.x, mLine.y, mLine.h, mLine.w, 20, 50, color);

    renderContext.drawRect(mBall.x, mBall.y, mBall.width, mBall.height, color);

    renderContext.drawText(std::to_string(mScoreP1.score), mScoreP1.x, mScoreP1.y, color);
    renderContext.drawText(std::to_string(mScoreP2.score), mScoreP2.x, mScoreP2.y, color);
}

void PongScreensaver::renderWin(RenderContext &renderContext) const
{
    (void)renderContext;
    if (mPlayer1Wins)
    {
        LOG_DEBUG() << "Player 1 wins!";
    }
    else
    {
        LOG_DEBUG() << "Player 2 wins!";
    }
}

void PongScreensaver::handleKey(const SDL_Event &event)
{
    bool isKeyDown = (event.type == SDL_EVENT_KEY_DOWN);
    
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

void PongScreensaver::updatePlayerMovement(const Window_Properties &windowProps)
{
    // Player 1 (W/S keys)
    if (mKeyW)
    {
        mPlayer1.y -= mPlayer1.velocity;
    }
    if (mKeyS)
    {
        mPlayer1.y += mPlayer1.velocity;
    }

    // Player 2 (Up/Down keys)
    if (mKeyUp)
    {
        mPlayer2.y -= mPlayer2.velocity;
    }
    if (mKeyDown)
    {
        mPlayer2.y += mPlayer2.velocity;
    }

    // Clamp paddle positions
    if (mPlayer2.y > windowProps.totalWindowHeight - mPlayer2.height)
    {
        mPlayer2.y = windowProps.totalWindowHeight - mPlayer2.height;
    }
    else if (mPlayer2.y < 0)
    {
        mPlayer2.y = 0;
    }
    if (mPlayer1.y > windowProps.totalWindowHeight - mPlayer1.height)
    {
        mPlayer1.y = windowProps.totalWindowHeight - mPlayer1.height;
    }
    else if (mPlayer1.y < 0)
    {
        mPlayer1.y = 0;
    }
}

bool PongScreensaver::isPlaying() const
{
    return mPlaying;
}

void PongScreensaver::initializePlayers(const Window_Properties &windowProps)
{
    float width = 20.0f;
    float height = 120.0f;
    float velocity = 10.0f;
    float y = (windowProps.totalWindowHeight - height) / 2.0f;
    mPlayer1.x = 20.0f;
    mPlayer1.y = y;
    mPlayer1.width = width;
    mPlayer1.height = height;
    mPlayer1.velocity = velocity;

    mPlayer2.x = windowProps.totalWindowWidth - width - 20.0f;
    mPlayer2.y = y;
    mPlayer2.width = width;
    mPlayer2.height = height;
    mPlayer2.velocity = velocity;

    float spacing = 20.0f;
    mLine.w = 5.0f;
    mLine.h = windowProps.totalWindowHeight - spacing;
    mLine.x = (windowProps.totalWindowWidth - mLine.w) / 2.0f;
    mLine.y = spacing;

    float ballWidth = 20.0f;
    mBall.x = (windowProps.totalWindowWidth - ballWidth) / 2.0f;
    mBall.y = (windowProps.totalWindowHeight - ballWidth) / 2.0f;
    mBall.width = ballWidth;
    mBall.height = ballWidth;
    mBall.dx = 5;
    mBall.dy = 5;

    mScoreP1.x = windowProps.totalWindowWidth / 4;
    mScoreP1.y = 50;

    mScoreP2.x = windowProps.totalWindowWidth - windowProps.totalWindowWidth / 4;
    mScoreP2.y = 50;
}

void PongScreensaver::handlePaddleCollision(const Paddle &paddle, bool isPlayer1)
{
    float ballCenterY = mBall.y + mBall.height / 2.0f;
    float paddleCenterY = paddle.y + paddle.height / 2.0f;
    float relativeIntersectY = (paddleCenterY - ballCenterY) / (paddle.height / 2.0f);
    
    // Clamp to -1 to 1 range to avoid extreme angles
    if (relativeIntersectY > 1.0f) relativeIntersectY = 1.0f;
    if (relativeIntersectY < -1.0f) relativeIntersectY = -1.0f;
    
    float maxAngleRadians = 75.0f * 3.14159265f / 180.0f;
    float exitAngle = -relativeIntersectY * maxAngleRadians;
    float ballSpeed = std::sqrt(mBall.dx * mBall.dx + mBall.dy * mBall.dy);
    ballSpeed *= 1.1;
    
    if (isPlayer1)
    {
        mBall.dx = ballSpeed * std::cos(exitAngle);
    }
    else
    {
        mBall.dx = -ballSpeed * std::cos(exitAngle);
    }
    
    mBall.dy = ballSpeed * std::sin(exitAngle);
}

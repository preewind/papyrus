#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_set>

#include "Screensaver.h"
#include "Core/types.h"
#include "random.h"
#include "ScreensaverAssets.h"

namespace
{
std::vector<uint32_t> sampleUniqueCellIndices(uint32_t totalCells, uint32_t sampleCount)
{
    if (sampleCount == 0 || totalCells == 0)
    {
        return {};
    }

    sampleCount = std::min(sampleCount, totalCells);
    std::unordered_set<uint32_t> selected;
    selected.reserve(sampleCount * 2);

    auto &rng = Random::get_engine();
    for (uint32_t j = totalCells - sampleCount; j < totalCells; ++j)
    {
        std::uniform_int_distribution<uint32_t> dist(0, j);
        const uint32_t t = dist(rng);
        if (!selected.insert(t).second)
        {
            selected.insert(j);
        }
    }

    std::vector<uint32_t> indices;
    indices.reserve(sampleCount);
    for (uint32_t idx : selected)
    {
        indices.push_back(idx);
    }
    std::shuffle(indices.begin(), indices.end(), rng);
    return indices;
}
} // namespace

Screensaver::Screensaver()
{
    mInactivityTimer = SDL_GetTicks();
    mLastFrameTimeMs = mInactivityTimer;
    mFrameTimeMs = mInactivityTimer;

    mEffects = {
        EffectGroup{
            EffectDef{
                ScreensaverAssets::HitMarker,
                ScreensaverAssets::HitMarkerPath,
                false,
                200,
                50.0f,
                50.0f,
                500,
                600,
                EffectPositionMode::Random,
            },
            {}},
        EffectGroup{
            EffectDef{
                ScreensaverAssets::Explosion,
                ScreensaverAssets::ExplosionPath,
                true,
                10,
                420.0f,
                420.0f,
                0,
                600,
                EffectPositionMode::Random,
            },
            {}},
        EffectGroup{
            EffectDef{
                ScreensaverAssets::Wow,
                ScreensaverAssets::WowPath,
                true,
                1,
                0.0f,
                0.0f,
                0,
                0,
                EffectPositionMode::Centered,
                2.0f,
            },
            {}},
        EffectGroup{
            EffectDef{
                ScreensaverAssets::Quickscope,
                ScreensaverAssets::QuickscopePath,
                true,
                5,
                0, 0,
                0,
                400,
                EffectPositionMode::RandomGrid,
                1.2f},
            {}

        },
    };
}

void Screensaver::updateScreensaver()
{
    uint64_t currTime = SDL_GetTicks();
    uint64_t timeToInactivity = (currTime - mInactivityTimer) / SDL_MS_PER_SECOND;
    if (timeToInactivity >= mInactivityInterval)
    {
        mInactive = true;
    }
}

void Screensaver::runScreensaver(const Window_Properties &windowProps)
{
    const uint32_t nowMs = SDL_GetTicks();
    const uint32_t deltaMs = nowMs - mLastFrameTimeMs;
    mLastFrameTimeMs = nowMs;
    mFrameTimeMs = nowMs;
    const float deltaSeconds = std::min(deltaMs / 1000.0f, 0.05f);

    if (mSuccess)
    {
        runSuccessScene(nowMs, deltaSeconds);
        return;
    }
    if (!mInitialized)
    {
        initializeLogo();
        mInitialized = true;
    }

    mLogo.x += mLogo.dx;
    mLogo.y += mLogo.dy;

    float maxX = windowProps.totalWindowWidth - mLogo.w;
    if (mLogo.x >= maxX)
    {
        mLogo.x = maxX;
        mLogo.dx *= -1;
    }
    else if (mLogo.x <= 0)
    {
        mLogo.x = 0;
        mLogo.dx *= -1;
    }
    float maxY = windowProps.totalWindowHeight - mLogo.h;
    if (mLogo.y >= maxY)
    {
        mLogo.y = maxY;
        mLogo.dy *= -1;
    }
    else if (mLogo.y <= 0)
    {
        mLogo.y = 0;
        mLogo.dy *= -1;
    }

    bool upperLeft = mLogo.x == 0 && mLogo.y == 0;
    bool upperRight = mLogo.x == maxX && mLogo.y == 0;
    bool lowerLeft = mLogo.x == 0 && mLogo.y == maxY;
    bool lowerRight = mLogo.x == maxX && mLogo.y == maxY;

    if (upperLeft || upperRight || lowerLeft || lowerRight)
    {
        startSuccessScene(windowProps, nowMs);
    }
}

void Screensaver::initializeLogo()
{
    mLogo.w = 386;
    mLogo.h = 180;
    mLogo.x = 438;
    mLogo.y = 270;
    mLogo.dx = 3;
    mLogo.dy = 3;
}

void Screensaver::startSuccessScene(const Window_Properties &windowProps, uint32_t nowMs)
{
    (void)nowMs;
    mSuccess = true;
    mSuccessAnimation.active = true;
    mSuccessAnimation.endX = mLogo.x;
    mSuccessAnimation.endY = mLogo.y - mLogo.h / 5;
    mSuccessAnimation.w = mLogo.w;
    mSuccessAnimation.h = mLogo.h;
    mSuccessAnimation.startX = (windowProps.totalWindowWidth - mSuccessAnimation.w) / 2;
    mSuccessAnimation.startY = (windowProps.totalWindowHeight - mSuccessAnimation.h) / 2;
    mSuccessAnimation.currentX = mSuccessAnimation.startX;
    mSuccessAnimation.currentY = mSuccessAnimation.startY;

    spawnSuccessEffects(windowProps);
}

void Screensaver::spawnSuccessEffects(const Window_Properties &windowProps)
{
    auto &rng = Random::get_engine();

    for (auto &group : mEffects)
    {
        const EffectDef &def = group.def;
        group.instances.clear();
        group.instances.reserve(def.count);

        const int maxX = std::max(0, static_cast<int>(windowProps.totalWindowWidth - def.w));
        const int maxY = std::max(0, static_cast<int>(windowProps.totalWindowHeight - def.h));
        std::uniform_int_distribution<int> xDist(0, maxX);
        std::uniform_int_distribution<int> yDist(0, maxY);
        std::uniform_int_distribution<uint32_t> offsetDist(0, def.maxOffsetMs);

        std::vector<uint32_t> cellPool;
        if (def.positionMode == EffectPositionMode::RandomGrid)
        {
            const uint32_t totalCells = def.count * def.count;
            cellPool = sampleUniqueCellIndices(totalCells, def.count);
        }

        for (size_t i = 0; i < def.count; ++i)
        {
            SuccessEffect effect;
            if (def.positionMode == EffectPositionMode::Centered)
            {
                effect.x = (windowProps.totalWindowWidth - def.w) / 2;
                effect.y = (windowProps.totalWindowHeight - def.h) / 2;
            }
            else if (def.positionMode == EffectPositionMode::RandomGrid)
            {
                uint32_t chosenCellIdx = cellPool[i];
                uint32_t cellX = chosenCellIdx % def.count;
                uint32_t cellY = chosenCellIdx / def.count;
                Vec2 pos = getRandomGridPos(cellX, cellY, def.count, windowProps.totalWindowWidth, windowProps.totalWindowHeight, def.w, def.h);
                effect.x = pos.x;
                effect.y = pos.y;
            }
            else
            {
                effect.x = static_cast<float>(xDist(rng));
                effect.y = static_cast<float>(yDist(rng));
            }

            effect.w = def.w;
            effect.h = def.h;
            effect.startOffset = def.maxOffsetMs > 0 ? offsetDist(rng) : 0;
            effect.duration = def.duration;
            group.instances.push_back(effect);
        }
    }
}

void Screensaver::runSuccessScene(uint32_t nowMs, float deltaSeconds)
{
    float dx = mSuccessAnimation.endX - mSuccessAnimation.currentX;
    float dy = mSuccessAnimation.endY - mSuccessAnimation.currentY;
    float length = std::sqrt(dx * dx + dy * dy);
    float movementStep = mSuccessAnimation.speedPixelsPerSecond * deltaSeconds;
    if (length <= movementStep)
    {
        mSuccessAnimation.currentX = mSuccessAnimation.endX;
        mSuccessAnimation.currentY = mSuccessAnimation.endY;

        if (mSuccessAnimation.active)
        {
            mSuccessAnimation.active = false;
            for (auto &group : mEffects)
                for (auto &e : group.instances)
                    e.startTime = nowMs + e.startOffset;
        }

        if (areAllEffectsFinished(nowMs))
        {
            mSuccess = false;
        }
    }
    else
    {
        mSuccessAnimation.currentX += (dx / length) * movementStep;
        mSuccessAnimation.currentY += (dy / length) * movementStep;
    }
}

bool Screensaver::areAllEffectsFinished(uint32_t nowMs) const
{
    for (const auto &group : mEffects)
    {
        for (const auto &effect : group.instances)
        {
            if (nowMs < effect.startTime + effect.duration)
            {
                return false;
            }
        }
    }

    return true;
}

// divide screen into grid of count*count cells,
Vec2 Screensaver::getRandomGridPos(uint32_t cellX, uint32_t cellY, uint32_t count, uint32_t screenWidth, uint32_t screenHeight, float spriteWidth, float spriteHeight) const
{
    if (count == 0)
    {
        return {0.0f, 0.0f};
    }

    float cellWidth = static_cast<float>(screenWidth) / count;
    float cellHeight = static_cast<float>(screenHeight) / count;

    float cellCenterX = (cellX * cellWidth) + (cellWidth / 2.0f);
    float cellCenterY = (cellY * cellHeight) + (cellHeight / 2.0f);

    float maxJitterX = std::max(0.0f, (cellWidth - spriteWidth) / 2.0f);
    float maxJitterY = std::max(0.0f, (cellHeight - spriteHeight) / 2.0f);

    float offsetX = Random::get_float(-maxJitterX, maxJitterX);
    float offsetY = Random::get_float(-maxJitterY, maxJitterY);

    const float rawX = cellCenterX + offsetX - (spriteWidth / 2.0f);
    const float rawY = cellCenterY + offsetY - (spriteHeight / 2.0f);

    const float maxX = std::max(0.0f, static_cast<float>(screenWidth) - spriteWidth);
    const float maxY = std::max(0.0f, static_cast<float>(screenHeight) - spriteHeight);

    return {std::clamp(rawX, 0.0f, maxX), std::clamp(rawY, 0.0f, maxY)};
}

bool Screensaver::isSuccess() const
{
    return mSuccess;
}

const SuccessAnimation &Screensaver::getSuccessAnimation() const
{
    return mSuccessAnimation;
}

const std::vector<EffectGroup> &Screensaver::getEffects() const
{
    return mEffects;
}

void Screensaver::resolveEffectDef(std::string_view assetName, uint32_t duration, float w, float h)
{
    for (auto &group : mEffects)
    {
        EffectDef &def = group.def;
        if (def.assetName != assetName)
        {
            continue;
        }
        if (def.duration == 0)
        {
            def.duration = duration;
        }
        if (def.w == 0.0f || def.h == 0.0f)
        {
            def.w = w * def.dimensionScale;
            def.h = h * def.dimensionScale;
        }

        break;
    }
}

uint32_t Screensaver::getFrameTimeMs() const
{
    return mFrameTimeMs;
}

bool Screensaver::isInactive() const
{
    return mInactive;
}

void Screensaver::resetTimer()
{
    mInactivityTimer = SDL_GetTicks();
    mInactive = false;
}

const Logo &Screensaver::getLogo() const
{
    return mLogo;
}

void Screensaver::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        resetTimer();
    }
}

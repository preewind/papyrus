#include <utility>
#include <cmath>

#include <SDL3_image/SDL_image.h>

#include "SDLRenderBackend.h"
#include "util.h"
#include "SDLUtil.h"

SDLRenderBackend::SDLRenderBackend(SDL_Window *window, const std::string &fontPath, uint8_t fontSize)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);
    CSP(mRenderer);

    setVSync(true);

    CSF(TTF_Init());

    mFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    CSP(mFont);

    // drawRainbowText();
}

SDLRenderBackend::~SDLRenderBackend()
{
    clearTextureCache();
    clearAnimationCache();

    if (mOverrideFont)
    {
        TTF_CloseFont(mOverrideFont);
    }
    if (mFont)
    {
        TTF_CloseFont(mFont);
    }
    TTF_Quit();

    if (mRenderer)
    {
        SDL_DestroyRenderer(mRenderer);
    }
}

void SDLRenderBackend::setVSync(bool enabled)
{
    CSF(SDL_SetRenderVSync(mRenderer, enabled ? 1 : 0));
}

void SDLRenderBackend::clear(const RenderColor &color)
{
    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));
    CSF(SDL_RenderClear(mRenderer));
}

void SDLRenderBackend::present()
{
    CSF(SDL_RenderPresent(mRenderer));
}

void SDLRenderBackend::fillRect(const Rect &rect, const RenderColor &color)
{
    SDL_FRect sdlRect;
    sdlRect.x = static_cast<float>(rect.x);
    sdlRect.y = static_cast<float>(rect.y);
    sdlRect.w = static_cast<float>(rect.w);
    sdlRect.h = static_cast<float>(rect.h);

    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));
    CSF(SDL_RenderFillRect(mRenderer, &sdlRect));
}

void SDLRenderBackend::fillRect(const RectF &rect, const RenderColor &color)
{
    SDL_FRect sdlRect;
    sdlRect.x = rect.x;
    sdlRect.y = rect.y;
    sdlRect.w = rect.w;
    sdlRect.h = rect.h;

    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));
    CSF(SDL_RenderFillRect(mRenderer, &sdlRect));
}

void SDLRenderBackend::drawDottedLine(float x, float y, float length, float lineWidth, float spacing, float rectSize, const RenderColor &color)
{
    uint32_t numRects = length / (rectSize+spacing);

    RectF line;
    line.x = x;
    line.w = lineWidth;
    line.h = rectSize;
    for (uint32_t i = 0; i < numRects; ++i)
    {
        line.y = y + i*(rectSize+spacing);
        fillRect(line, color);
    }
    
}

void SDLRenderBackend::setClipRect(const Rect &rect)
{
    SDL_Rect sdlRect;
    sdlRect.x = rect.x;
    sdlRect.y = rect.y;
    sdlRect.w = rect.w;
    sdlRect.h = rect.h;

    CSF(SDL_SetRenderClipRect(mRenderer, &sdlRect));
}

void SDLRenderBackend::clearClipRect()
{
    CSF(SDL_SetRenderClipRect(mRenderer, nullptr));
}

void SDLRenderBackend::drawText(const std::string &text, int x, int y, const RenderColor &color)
{
    if (text.empty())
    {
        return;
    }

    SDL_Color sdlColor;
    sdlColor.r = color.r;
    sdlColor.g = color.g;
    sdlColor.b = color.b;
    sdlColor.a = color.a;

    SDL_Surface *surface = TTF_RenderText_Blended(mFont, text.c_str(), text.size(), sdlColor);
    CSP(surface);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    CSP(texture);

    SDL_FRect dst;
    dst.x = static_cast<float>(x);
    dst.y = static_cast<float>(y);
    dst.w = static_cast<float>(surface->w);
    dst.h = static_cast<float>(surface->h);

    CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void SDLRenderBackend::rainbowText()
{
    const char *fontPath = "assets/impact.ttf";
    TTF_Font *localFont = TTF_OpenFont(fontPath, 72);
    CSP(localFont);

    std::string text = "SWAG";

    size_t frameCount = 60;

    SDL_Surface **frames = new SDL_Surface *[frameCount];
    int *delays = new int[frameCount];

    for (size_t i = 0; i < frameCount; ++i)
    {
        float noise = (i * 123.456f);
        float hue = fmod(std::fabs(noise), 360.0f);
        SDL_Color mlgColor = HSVtoRGB(hue, 1.0f, 1.0f);
        frames[i] = TTF_RenderText_Blended(localFont, text.c_str(), text.size(), mlgColor);
        CSP(frames[i]);

        delays[i] = 30;
    }

    IMG_Animation anim;
    anim.w = frames[frameCount - 1]->w;
    anim.h = frames[frameCount - 1]->h;
    anim.count = frameCount;
    anim.frames = frames;
    anim.delays = delays;

    IMG_SaveAnimation(&anim, "rainbow.gif");

    for (size_t i = 0; i < frameCount; ++i)
    {
        SDL_DestroySurface(frames[i]);
    }
    delete[] frames;
    delete[] delays;
}

void SDLRenderBackend::loadOverrideFont(const std::string &fontPath, uint8_t fontSize)
{
    if (mOverrideFont)
    {
        TTF_CloseFont(mOverrideFont);
        mOverrideFont = nullptr;
    }
    mOverrideFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    CSP(mOverrideFont);
}

void SDLRenderBackend::drawRainbowText(const std::string &text, int x, int y)
{
    if (text.empty())
    {
        return;
    }

    TTF_Font *drawFont = mOverrideFont ? mOverrideFont : mFont;
    int penX = x;
    const RainbowTextConfig &cfg = mRainbowConfig;
    const float timeHue = static_cast<float>(SDL_GetTicks()) * cfg.speed;
    const float linePhase = static_cast<float>(y) * cfg.linePhase;
    const int layoutLineHeight = TTF_GetFontHeight(mFont);
    const int drawLineHeight = TTF_GetFontHeight(drawFont);
    const float lineOffsetY = static_cast<float>(layoutLineHeight - drawLineHeight) * 0.5f;

    for (size_t i = 0; i < text.size(); ++i)
    {
        std::string glyph(1, text[i]);
        const float hue = std::fmod(std::fabs(timeHue + linePhase + static_cast<float>(i) * cfg.charSpread), 360.0f);
        SDL_Color mlgColor = HSVtoRGB(hue, 1.0f, 1.0f);

        SDL_Surface *surface = TTF_RenderText_Blended(drawFont, glyph.c_str(), glyph.size(), mlgColor);
        CSP(surface);

        SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
        CSP(texture);

        const float wobbleSeed = static_cast<float>(SDL_GetTicks()) * 0.008f + static_cast<float>(i) * 1.73f + static_cast<float>(y) * 0.11f;
        const float xWobble = cfg.wobbleX ? std::sin(wobbleSeed) * 3.0f : 0.0f;
        const float yWobble = cfg.wobbleY ? std::cos(wobbleSeed * 0.83f) * 2.0f : 0.0f;
        const int spacingJitter = cfg.wobbleSpacing ? static_cast<int>(std::round((std::sin(wobbleSeed * 1.3f) + 1.0f) * 0.5f)) : 0;
        int layoutGlyphWidth = surface->w;
        int layoutGlyphHeight = 0;
        CSF(TTF_GetStringSize(mFont, glyph.c_str(), glyph.size(), &layoutGlyphWidth, &layoutGlyphHeight));
        const float slotOffsetX = static_cast<float>(layoutGlyphWidth - surface->w) * 0.5f;

        SDL_FRect dst;
        dst.x = static_cast<float>(penX) + slotOffsetX + xWobble;
        dst.y = static_cast<float>(y) + lineOffsetY + yWobble;
        dst.w = static_cast<float>(surface->w);
        dst.h = static_cast<float>(surface->h);

        CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));

        penX += layoutGlyphWidth + spacingJitter;

        SDL_DestroyTexture(texture);
        SDL_DestroySurface(surface);
    }
}

void SDLRenderBackend::loadTexture(float x, float y, float w, float h, const std::filesystem::path &file, float rotation)
{
    SDL_Texture *texture = loadAndCacheTexture(file);
    if (texture == nullptr)
    {
        return;
    }

    SDL_FRect dst{x, y, w, h};
    CSF(SDL_RenderTextureRotated(mRenderer, texture, nullptr, &dst, rotation, nullptr, SDL_FLIP_NONE));
}

void SDLRenderBackend::loadAnimation(float x, float y, float w, float h, const std::filesystem::path &file, uint32_t elapsedMs, AnimationPlaybackMode playbackMode, float rotation)
{
    AnimationData *animation = loadAndCacheAnimation(file);
    if (animation == nullptr || animation->frames.empty())
    {
        return;
    }

    bool shouldRender = true;
    const uint32_t frameIndex = selectAnimationFrameIndex(*animation, elapsedMs, playbackMode, shouldRender);
    if (!shouldRender)
    {
        return;
    }

    SDL_Texture *texture = animation->frames[frameIndex].texture;
    if (texture == nullptr)
    {
        return;
    }

    SDL_FRect dst{x, y, w, h};
    CSF(SDL_RenderTextureRotated(mRenderer, texture, nullptr, &dst, rotation, nullptr, SDL_FLIP_NONE));
}

bool SDLRenderBackend::preloadTexture(const std::filesystem::path &file)
{
    return loadAndCacheTexture(file) != nullptr;
}

bool SDLRenderBackend::preloadAnimation(const std::filesystem::path &file)
{
    AnimationData *animation = loadAndCacheAnimation(file);
    return animation != nullptr && !animation->frames.empty();
}

void SDLRenderBackend::evictTexture(const std::filesystem::path &file)
{
    const std::string key = textureKey(file);
    auto textureEntry = mTextureCache.find(key);
    if (textureEntry == mTextureCache.end())
    {
        return;
    }

    SDL_DestroyTexture(textureEntry->second);
    mTextureCache.erase(textureEntry);
}

void SDLRenderBackend::evictAnimation(const std::filesystem::path &file)
{
    const std::string key = textureKey(file);
    auto animationEntry = mAnimationCache.find(key);
    if (animationEntry == mAnimationCache.end())
    {
        return;
    }

    destroyAnimationData(animationEntry->second);
    mAnimationCache.erase(animationEntry);
}

void SDLRenderBackend::clearTextureCache()
{
    for (auto &[key, texture] : mTextureCache)
    {
        (void)key;
        SDL_DestroyTexture(texture);
    }

    mTextureCache.clear();
}

void SDLRenderBackend::clearAnimationCache()
{
    for (auto &[key, animation] : mAnimationCache)
    {
        (void)key;
        destroyAnimationData(animation);
    }

    mAnimationCache.clear();
}

SDL_Texture *SDLRenderBackend::loadAndCacheTexture(const std::filesystem::path &file)
{
    const std::string key = textureKey(file);
    auto textureEntry = mTextureCache.find(key);
    if (textureEntry != mTextureCache.end())
    {
        return textureEntry->second;
    }

    SDL_Texture *texture = IMG_LoadTexture(mRenderer, key.c_str());
    if (texture == nullptr)
    {
        CSP(texture);
        return nullptr;
    }

    CSF(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));

    mTextureCache.emplace(key, texture);
    return texture;
}

SDLRenderBackend::AnimationData *SDLRenderBackend::loadAndCacheAnimation(const std::filesystem::path &file)
{
    const std::string key = textureKey(file);
    auto animationEntry = mAnimationCache.find(key);
    if (animationEntry != mAnimationCache.end())
    {
        return &animationEntry->second;
    }

    IMG_Animation *rawAnimation = IMG_LoadAnimation(key.c_str());
    if (rawAnimation == nullptr)
    {
        CSP(rawAnimation);
        return nullptr;
    }

    AnimationData animationData;
    animationData.frames.reserve(static_cast<size_t>(rawAnimation->count));

    for (int i = 0; i < rawAnimation->count; ++i)
    {
        SDL_Surface *surface = rawAnimation->frames[i];
        if (surface == nullptr)
        {
            continue;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
        if (texture == nullptr)
        {
            CSP(texture);
            continue;
        }

        CSF(SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));

        const int rawDelay = rawAnimation->delays != nullptr ? rawAnimation->delays[i] : 0;
        const uint32_t delayMs = rawDelay > 0 ? static_cast<uint32_t>(rawDelay) : 100;

        animationData.frames.push_back(AnimationFrame{texture, delayMs});
        animationData.totalDurationMs += delayMs;
    }

    IMG_FreeAnimation(rawAnimation);

    if (animationData.frames.empty())
    {
        return nullptr;
    }

    auto [insertedIt, inserted] = mAnimationCache.emplace(key, std::move(animationData));
    (void)inserted;
    return &insertedIt->second;
}

std::string SDLRenderBackend::textureKey(const std::filesystem::path &file)
{
    return file.lexically_normal().string();
}

void SDLRenderBackend::destroyAnimationData(AnimationData &animation)
{
    for (auto &frame : animation.frames)
    {
        if (frame.texture != nullptr)
        {
            SDL_DestroyTexture(frame.texture);
            frame.texture = nullptr;
        }
    }

    animation.frames.clear();
    animation.totalDurationMs = 0;
}

uint32_t SDLRenderBackend::selectAnimationFrameIndex(const AnimationData &animation, uint32_t elapsedMs, AnimationPlaybackMode playbackMode, bool &shouldRender)
{
    shouldRender = true;
    if (animation.frames.empty())
    {
        shouldRender = false;
        return 0;
    }

    const uint32_t totalDuration = animation.totalDurationMs;
    if (totalDuration == 0)
    {
        return 0;
    }

    uint32_t localElapsed = elapsedMs;
    switch (playbackMode)
    {
    case AnimationPlaybackMode::Loop:
        localElapsed %= totalDuration;
        break;
    case AnimationPlaybackMode::HoldLastFrame:
        if (localElapsed >= totalDuration)
        {
            return static_cast<uint32_t>(animation.frames.size() - 1);
        }
        break;
    case AnimationPlaybackMode::HideAfterEnd:
        if (localElapsed >= totalDuration)
        {
            shouldRender = false;
            return 0;
        }
        break;
    }

    uint32_t accumulated = 0;
    for (uint32_t i = 0; i < animation.frames.size(); ++i)
    {
        accumulated += animation.frames[i].delayMs;
        if (localElapsed < accumulated)
        {
            return i;
        }
    }

    return static_cast<uint32_t>(animation.frames.size() - 1);
}

uint32_t SDLRenderBackend::getAnimationDurationMs(const std::filesystem::path &file) const
{
    const std::string key = textureKey(file);
    auto animationEntry = mAnimationCache.find(key);
    if (animationEntry == mAnimationCache.end())
    {
        return 0;
    }

    return animationEntry->second.totalDurationMs;
}

std::pair<uint32_t, uint32_t> SDLRenderBackend::getAnimationDimensions(const std::filesystem::path &file) const
{
    const std::string key = textureKey(file);
    auto animationEntry = mAnimationCache.find(key);
    if (animationEntry == mAnimationCache.end() || animationEntry->second.frames.empty())
    {
        return {0, 0};
    }

    const AnimationData &animation = animationEntry->second;
    SDL_Texture *firstTexture = animation.frames[0].texture;
    if (firstTexture == nullptr)
    {
        return {0, 0};
    }

    float w = 0.0f;
    float h = 0.0f;
    CSF(SDL_GetTextureSize(firstTexture, &w, &h));
    return {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
}

uint32_t SDLRenderBackend::width(std::string_view text) const
{
    if (text.empty())
    {
        return 0;
    }

    int w = 0;
    int h = 0;
    CSF(TTF_GetStringSize(mFont, text.data(), text.size(), &w, &h));
    return static_cast<uint32_t>(w);
}

int SDLRenderBackend::lineHeight() const
{
    return TTF_GetFontHeight(mFont);
}

void SDLRenderBackend::setFontSize(uint8_t size)
{
    CSF(TTF_SetFontSize(mFont, size));
    if (mOverrideFont)
    {
        CSF(TTF_SetFontSize(mOverrideFont, size));
    }
}

SDL_Renderer *SDLRenderBackend::nativeRenderer() const
{
    return mRenderer;
}

#include "SDLRenderBackend.h"

#include "util.h"

#include <SDL3_image/SDL_image.h>

#include <utility>

SDLRenderBackend::SDLRenderBackend(SDL_Window *window, const std::string &fontPath, uint8_t fontSize)
{
    mRenderer = SDL_CreateRenderer(window, nullptr);
    CSP(mRenderer);

    setVSync(true);

    CSF(TTF_Init());

    mFont = TTF_OpenFont(fontPath.c_str(), fontSize);
    CSP(mFont);
}

SDLRenderBackend::~SDLRenderBackend()
{
    clearTextureCache();
    clearAnimationCache();

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

void SDLRenderBackend::fillRect(const RenderRect &rect, const RenderColor &color)
{
    SDL_FRect sdlRect;
    sdlRect.x = static_cast<float>(rect.x);
    sdlRect.y = static_cast<float>(rect.y);
    sdlRect.w = static_cast<float>(rect.w);
    sdlRect.h = static_cast<float>(rect.h);

    CSF(SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a));
    CSF(SDL_RenderFillRect(mRenderer, &sdlRect));
}

void SDLRenderBackend::setClipRect(const RenderRect &rect)
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

void SDLRenderBackend::loadTexture(float x, float y, float w, float h, const std::filesystem::path &file)
{
    SDL_Texture *texture = loadAndCacheTexture(file);
    if (texture == nullptr)
    {
        return;
    }

    SDL_FRect dst{x, y, w, h};
    CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));
}

void SDLRenderBackend::loadAnimation(float x, float y, float w, float h,
                                     const std::filesystem::path &file,
                                     uint32_t elapsedMs,
                                     AnimationPlaybackMode playbackMode)
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
    CSF(SDL_RenderTexture(mRenderer, texture, nullptr, &dst));
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

uint32_t SDLRenderBackend::selectAnimationFrameIndex(const AnimationData &animation,
                                                     uint32_t elapsedMs,
                                                     AnimationPlaybackMode playbackMode,
                                                     bool &shouldRender)
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
}

SDL_Renderer *SDLRenderBackend::nativeRenderer() const
{
    return mRenderer;
}

#pragma once

#include <SDL3/SDL_pixels.h>

#include "types.h"
#include "util.h"

struct Theme
{
    SDL_Color background{0, 0, 0, 255};
    SDL_Color text{255, 255, 255, 255};
    SDL_Color cursor{255, 255, 255, 255};
    SDL_Color selection{46, 47, 108, 255};
    SDL_Color searchMatch{46, 47, 108, 255};
    SDL_Color terminalBackground{31, 32, 33, 255};
    SDL_Color terminalCursor{101, 102, 103, 255};
    SDL_Color lineNumbers{66, 67, 68, 255};
    SDL_Color overlayBackground{34, 35, 36, 255};
    SDL_Color fileBrowserDir = {255, 255, 0, 255};
};

struct LexerTheme
{
    SDL_Color Punctuation = {255, 255, 0, 255};
    SDL_Color Comment = hexToSDLColor("#8B949E");
    SDL_Color String = hexToSDLColor("#A5D6FF");
    SDL_Color Keyword = hexToSDLColor("#FF7B72");
    SDL_Color Preprocessor = hexToSDLColor("#C586C0");
    SDL_Color IncludeLib = hexToSDLColor("#A5D6FF");
    SDL_Color Default = {255, 255, 255, 255};
};
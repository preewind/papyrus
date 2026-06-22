#pragma once

#include "RenderTypes.h"

struct Theme
{
    RenderColor background{0, 0, 0, 255};
    RenderColor text{255, 255, 255, 255};
    RenderColor cursor{255, 255, 255, 255};
    RenderColor selection{46, 47, 108, 255};
    RenderColor searchMatch{46, 47, 108, 255};
    RenderColor terminalBackground{31, 32, 33, 255};
    RenderColor terminalCursor{101, 102, 103, 255};
    RenderColor lineNumbers{66, 67, 68, 255};
    RenderColor overlayBackground{34, 35, 36, 255};
    RenderColor fileBrowserDir = {255, 255, 0, 255};
    RenderColor fileBrowserUnsupported{220, 80, 80, 255};
};

struct LexerTheme
{
    RenderColor punctuation = {255, 255, 0, 255};
    RenderColor comment = {139, 148, 158, 255};
    RenderColor string = {165, 214, 255, 255};
    RenderColor keyword = {255, 123, 114, 255};
    RenderColor preprocessor = {197, 134, 192, 255};
    RenderColor includeLib = {165, 214, 255, 255};
    RenderColor Default = {255, 255, 255, 255};
};
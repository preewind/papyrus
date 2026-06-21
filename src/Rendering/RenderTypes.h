#pragma once

#include <cstdint>

struct RenderColor
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;
};

struct RenderRect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

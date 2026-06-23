#pragma once

#include <cstdint>

struct ScrollViewport
{
    int offsetX = 0;
    int visibleWidth = 0;
    uint32_t offsetY = 0;
    uint32_t visibleRows = 0;

    void ensureVisible(int x, int padding = 0)
    {
        if (x < offsetX)
        {
            offsetX = x;
        }
        else if (x > offsetX + visibleWidth)
        {
            offsetX = x - visibleWidth + padding;
        }
    }

    void ensureVisibleRow(uint32_t row)
    {
        if (row < offsetY)
        {
            offsetY = row;
        }
        else if (visibleRows != 0 && row >= offsetY + visibleRows)
        {
            offsetY = row - visibleRows + 1;
        }
    }
};

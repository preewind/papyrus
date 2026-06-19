#pragma once

struct ScrollViewport
{
    int offsetX = 0;
    int visibleWidth = 0;

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
};

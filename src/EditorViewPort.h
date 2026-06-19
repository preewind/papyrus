#pragma once

#include "TextLayout.h"
#include "LayoutManager.h"

class Editor;

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
class EditorViewport
{
public:
    
    void updateHorizontal(const Editor &editor, const TextLayout &textLayout, const LayoutConfig &layout, const LayoutInput &input);

    int scrollX() const;

private:
    ScrollViewport mHorizontal;
};
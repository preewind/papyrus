#pragma once

#include "TextLayout.h"
#include "LayoutManager.h"
#include "Viewport.h"

class Editor;

class EditorViewport
{
public:
    void updateHorizontal(const Editor &editor, const TextLayout &textLayout, const LayoutConfig &layout, const LayoutInput &input);
    void updateVertical(const Editor &editor, uint32_t visibleRows);

    int scrollX() const;
    uint32_t scrollY() const;

private:
    ScrollViewport mHorizontal;
    uint32_t mScrollOffsetY = 0;
    uint32_t mVisibleRows = 0;
};
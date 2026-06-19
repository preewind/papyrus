#pragma once

#include "TextLayout.h"
#include "LayoutManager.h"
#include "Viewport.h"

class Editor;

class EditorViewport
{
public:
    
    void updateHorizontal(const Editor &editor, const TextLayout &textLayout, const LayoutConfig &layout, const LayoutInput &input);

    int scrollX() const;

private:
    ScrollViewport mHorizontal;
};
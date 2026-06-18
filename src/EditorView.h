#pragma once

#include "Editor.h"

class Renderer;

class EditorView{

public:
    void render(Renderer& r, const Editor& editor);

    void renderLineNumbers(Renderer &renderer, uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows);
    void renderSelection(Renderer &renderer, const Editor &editor);
    void renderCursor(Renderer &renderer, const Editor &editor);
    void renderText(Renderer &renderer, const Editor &editor);
};
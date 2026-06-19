#pragma once

#include "Editor.h"
#include "EditorViewPort.h"
#include "TextLayout.h"

class Renderer;

class EditorView
{

public:
    void render(Renderer &renderer, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout);

private:
    
    void renderLineNumbers(Renderer &renderer, uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows, const TextLayout &textLayout);
    void renderSelection(Renderer &renderer, const Editor &editor, const EditorViewport &viewport);
    void renderCursor(Renderer &renderer, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout);
    void renderText(Renderer &renderer, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout);
    void renderSearchMatches(Renderer &renderer, const Editor &editor, const EditorViewport &viewport);
};
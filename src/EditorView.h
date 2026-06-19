#pragma once

#include "Editor.h"
#include "EditorViewPort.h"
#include "TextLayout.h"
#include "LayoutManager.h"

class Renderer;

class EditorView
{

public:
    void render(Renderer &renderer, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const EditorLayout &editorLayout);

private:
    
    void renderLineNumbers(Renderer &renderer, uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows, const TextLayout &textLayout, const LayoutConfig &layoutConfig);
    void renderSelection(Renderer &renderer, const Editor &editor, const EditorViewport &viewport);
    void renderCursor(Renderer &renderer, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig);
    void renderText(Renderer &renderer, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig);
    void renderSearchMatches(Renderer &renderer, const Editor &editor, const EditorViewport &viewport);
};
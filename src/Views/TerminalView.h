#pragma once

#include "Editor.h"
#include "LayoutManager.h"
#include "RenderContext.h"

#include "TextLayout.h"

class TerminalView
{
public:
    void render(RenderContext &renderContext, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties);

private:
    void renderTerminal(RenderContext &renderContext, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties);
    void renderTerminalCursor(RenderContext &renderContext, const Terminal &terminal, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties);
    void renderTerminalSelection(RenderContext &renderContext, const Terminal &terminal, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties);
};
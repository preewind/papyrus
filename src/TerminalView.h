#pragma once

#include "Editor.h"
#include "TextLayout.h"

class Renderer;

class TerminalView
{
public:
    void render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout);

private:
    void renderTerminal(Renderer &renderer, const Editor &editor, const TextLayout &textLayout);
    void renderTerminalCursor(Renderer &renderer, const Terminal &terminal, const TextLayout &textLayout);
};
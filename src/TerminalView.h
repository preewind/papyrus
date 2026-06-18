#pragma once

#include "Editor.h"

class Renderer;

class TerminalView
{
public:
    void render(Renderer &renderer, const Editor &editor);
private:
    void renderTerminal(Renderer &renderer, const Editor &editor);
    void renderTerminalCursor(Renderer &renderer, const Terminal &terminal);
};
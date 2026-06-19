#pragma once

#include "Editor.h"
#include "TextLayout.h"
#include "LayoutManager.h"

class Renderer;

class TerminalView
{
public:
    void render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const SDL_Properties &sdlProps);

private:
    void renderTerminal(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const SDL_Properties &sdlProps);
    void renderTerminalCursor(Renderer &renderer, const Terminal &terminal, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const SDL_Properties &sdlProps);
};
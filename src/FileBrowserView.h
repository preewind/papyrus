#pragma once

#include "FileBrowser.h"
#include "TextLayout.h"
#include "LayoutManager.h"

class Renderer;

class FileBrowserView
{
public:
    void render(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps);

private:
    void renderFileBrowserSelection(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps);
    void renderFileBrowser(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps);
};
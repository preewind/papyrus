#pragma once

#include "FileBrowser.h"
#include "TextLayout.h"

class Renderer;

class FileBrowserView
{
public:
    void render(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout);

private:
    void renderFileBrowserSelection(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout);
    void renderFileBrowser(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout);
};
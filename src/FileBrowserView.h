#pragma once

#include "FileBrowser.h"

class Renderer;

class FileBrowserView{
    public:
    void render(Renderer &renderer, FileBrowser &browser);
private:
    void renderFileBrowserSelection(Renderer &renderer, FileBrowser &browser);
    void renderFileBrowser(Renderer &renderer, FileBrowser &browser);
};
#pragma once

#include "Editor.h"

class Renderer;

class SearchView {
    public:
    void render(Renderer& renderer, const Editor& editor);
    void renderSearchOverlay(Renderer &renderer, const SearchSession &session);
    void renderSearchCursor(Renderer &renderer, const SearchSession &session);
};
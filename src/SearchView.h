#pragma once

#include "Editor.h"
#include "TextLayout.h"

class Renderer;

class SearchView
{
public:
    void render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout);

private:
    void renderSearchOverlay(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout);
    void renderSearchCursor(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout);
};
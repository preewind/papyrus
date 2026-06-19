#pragma once

#include "Editor.h"
#include "TextLayout.h"
#include "LayoutManager.h"

class Renderer;

class SearchView
{
public:
    void render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const SearchLayout &searchLayout);

private:
    void renderSearchOverlay(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout);
    void renderSearchCursor(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout);
};
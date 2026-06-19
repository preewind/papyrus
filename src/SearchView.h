#pragma once

#include "Editor.h"
#include "TextLayout.h"
#include "LayoutManager.h"
#include "SearchViewPort.h"

class Renderer;

class SearchView
{
public:
    void render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport);

private:
    void renderSearchOverlay(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport);
    void renderSearchCursor(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport);
};
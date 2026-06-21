#pragma once

#include "Editor.h"
#include "LayoutManager.h"
#include "RenderContext.h"
#include "SearchViewPort.h"

#include "TextLayout.h"

class SearchView
{
public:
    void render(RenderContext &renderContext, const Editor &editor, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport, bool cursorVisible);

private:
    void renderSearchOverlay(RenderContext &renderContext, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport);
    void renderSearchSelection(RenderContext &renderContext, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport);
    void renderSearchText(RenderContext &renderContext, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport);
    void renderSearchCursor(RenderContext &renderContext, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport, bool cursorVisible);
};
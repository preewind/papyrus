#pragma once

#include "SearchSession.h"
#include "TextLayout.h"
#include "LayoutManager.h"
#include "Viewport.h"

class SearchViewport
{
public:
    void updateHorizontal(const SearchSession &session, const TextLayout &textLayout, const SearchLayout &layout);

    int scrollX() const;

private:
    ScrollViewport mHorizontal;
};

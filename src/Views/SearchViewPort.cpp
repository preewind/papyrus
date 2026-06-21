#include "SearchViewPort.h"

void SearchViewport::updateHorizontal(const SearchSession &session, const TextLayout &textLayout, const SearchLayout &layout)
{
    uint32_t cursorTextWidth = textLayout.width(session.getQuery().substr(0, session.getCursor()));
    mHorizontal.visibleWidth = static_cast<int>(layout.queryBox.w) - static_cast<int>(layout.textPadding * 2);
    mHorizontal.ensureVisible(static_cast<int>(cursorTextWidth), 2);
}

int SearchViewport::scrollX() const
{
    return mHorizontal.offsetX;
}

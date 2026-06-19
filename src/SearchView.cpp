#include "SearchView.h"

#include "Renderer.h"

void SearchView::render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport)
{
    if (editor.isSearchActive())
    {
        renderSearchOverlay(renderer, editor.getSearch(), textLayout, searchLayout, viewport);
        renderSearchCursor(renderer, editor.getSearch(), textLayout, searchLayout, viewport);
    }
}

void SearchView::renderSearchOverlay(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport)
{
    const auto &theme = renderer.getTheme();
    uint32_t currMatch = session.hasMatches() ? session.getCurrentMatchIndex() + 1 : 0;
    const std::string &matchStr = std::to_string(currMatch) + "/" + std::to_string(session.getMatches().size());
    uint32_t matchBoxWidth = textLayout.width(matchStr) + searchLayout.matchBoxPadding;

    renderer.drawRect(searchLayout.queryBox, theme.overlayBackground);
    renderer.drawRect(searchLayout.matchBox.x, searchLayout.matchBox.y, matchBoxWidth, searchLayout.matchBox.h, theme.overlayBackground);

    Rect searchClipRect{
        searchLayout.queryBox.x + searchLayout.textPadding,
        searchLayout.queryBox.y,
        searchLayout.queryBox.w - (searchLayout.textPadding * 2),
        searchLayout.queryBox.h};
    renderer.pushClipRect(searchClipRect);

    const std::string &query = session.getQuery();
    renderer.drawText(query, searchLayout.textX - viewport.scrollX(), searchLayout.textY);
    renderer.clearClipRect();
    renderer.drawText(matchStr, searchLayout.matchBoxTextX, searchLayout.textY);
}

void SearchView::renderSearchCursor(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport)
{
    const auto &editorLayout = renderer.getSDL_Properties();
    const auto &theme = renderer.getTheme();
    if (renderer.getCursorBlinker().visible())
    {
        uint32_t cursorTextWidth = textLayout.width(session.getQuery().substr(0, session.getCursor()));
        int cursorX = searchLayout.queryBox.x + searchLayout.textPadding + cursorTextWidth - viewport.scrollX();
        Rect searchClipRect{
            searchLayout.queryBox.x + searchLayout.textPadding,
            searchLayout.queryBox.y,
            searchLayout.queryBox.w - (searchLayout.textPadding * 2),
            searchLayout.queryBox.h};
        renderer.pushClipRect(searchClipRect);
        renderer.drawRect(cursorX, searchLayout.textY, 2, editorLayout.lineHeight, theme.cursor);
        renderer.clearClipRect();
    }
}

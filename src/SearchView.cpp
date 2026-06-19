#include "SearchView.h"

void SearchView::render(RenderContext &renderContext, const Editor &editor, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport, bool cursorVisible)
{
    if (editor.isSearchActive())
    {
        renderSearchOverlay(renderContext, editor.getSearch(), textLayout, searchLayout, viewport);
        renderSearchCursor(renderContext, editor.getSearch(), textLayout, searchLayout, viewport, cursorVisible);
    }
}

void SearchView::renderSearchOverlay(RenderContext &renderContext, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport)
{
    const auto &theme = renderContext.getTheme();
    uint32_t currMatch = session.hasMatches() ? session.getCurrentMatchIndex() + 1 : 0;
    const std::string &matchStr = std::to_string(currMatch) + "/" + std::to_string(session.getMatches().size());
    uint32_t matchBoxWidth = textLayout.width(matchStr) + searchLayout.matchBoxPadding;

    renderContext.drawRect(searchLayout.queryBox, theme.overlayBackground);
    renderContext.drawRect(searchLayout.matchBox.x, searchLayout.matchBox.y, matchBoxWidth, searchLayout.matchBox.h, theme.overlayBackground);

    Rect searchClipRect{
        searchLayout.queryBox.x + searchLayout.textPadding,
        searchLayout.queryBox.y,
        searchLayout.queryBox.w - (searchLayout.textPadding * 2),
        searchLayout.queryBox.h};
    renderContext.pushClipRect(searchClipRect);

    const std::string &query = session.getQuery();
    renderContext.drawText(query, searchLayout.textX - viewport.scrollX(), searchLayout.textY);
    renderContext.clearClipRect();
    renderContext.drawText(matchStr, searchLayout.matchBoxTextX, searchLayout.textY);
}

void SearchView::renderSearchCursor(RenderContext &renderContext, const SearchSession &session, const TextLayout &textLayout, const SearchLayout &searchLayout, const SearchViewport &viewport, bool cursorVisible)
{
    const auto &editorLayout = renderContext.getSDL_Properties();
    const auto &theme = renderContext.getTheme();
    if (cursorVisible)
    {
        uint32_t cursorTextWidth = textLayout.width(session.getQuery().substr(0, session.getCursor()));
        int cursorX = searchLayout.queryBox.x + searchLayout.textPadding + cursorTextWidth - viewport.scrollX();
        Rect searchClipRect{
            searchLayout.queryBox.x + searchLayout.textPadding,
            searchLayout.queryBox.y,
            searchLayout.queryBox.w - (searchLayout.textPadding * 2),
            searchLayout.queryBox.h};
        renderContext.pushClipRect(searchClipRect);
        renderContext.drawRect(cursorX, searchLayout.textY, 2, editorLayout.lineHeight, theme.cursor);
        renderContext.clearClipRect();
    }
}

#include "SearchView.h"

#include "Renderer.h"

void SearchView::render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout)
{
    if (editor.isSearchActive())
    {
        renderSearchOverlay(renderer, editor.getSearch(), textLayout);
        renderSearchCursor(renderer, editor.getSearch(), textLayout);
    }
}

void SearchView::renderSearchOverlay(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout)
{
    const auto &layout = renderer.getSearchLayout();
    const auto &theme = renderer.getTheme();
    uint32_t currMatch = session.hasMatches() ? session.getCurrentMatchIndex() + 1 : 0;
    const std::string &matchStr = std::to_string(currMatch) + "/" + std::to_string(session.getMatches().size());
    uint32_t matchBoxWidth = textLayout.width(matchStr) + layout.matchBoxPadding;

    renderer.drawRect(layout.queryBox, theme.overlayBackground);
    renderer.drawRect(layout.matchBox.x, layout.matchBox.y, matchBoxWidth, layout.matchBox.h, theme.overlayBackground);

    SDL_Rect searchClipRect{
        static_cast<int>(layout.queryBox.x + layout.textPadding),
        static_cast<int>(layout.queryBox.y),
        static_cast<int>(layout.queryBox.w - (layout.textPadding * 2)),
        static_cast<int>(layout.queryBox.h)};
    renderer.pushClipRect(searchClipRect);

    const std::string &query = session.getQuery();
    renderer.drawText(query, layout.textX - renderer.getScrollOffsetXSearch(), layout.textY);
    renderer.clearClipRect();
    renderer.drawText(matchStr, layout.matchBoxTextX, layout.textY);
}

void SearchView::renderSearchCursor(Renderer &renderer, const SearchSession &session, const TextLayout &textLayout)
{
    const auto &layout = renderer.getSearchLayout();
    const auto &editorLayout = renderer.getSDL_Properties();
    const auto &theme = renderer.getTheme();
    if (renderer.getCursorBlinker().visible())
    {
        uint32_t cursorTextWidth = textLayout.width(session.getQuery().substr(0, session.getCursor()));
        int cursorX = layout.queryBox.x + layout.textPadding + cursorTextWidth - renderer.getScrollOffsetXSearch();
        SDL_Rect searchClipRect{
            static_cast<int>(layout.queryBox.x + layout.textPadding),
            static_cast<int>(layout.queryBox.y),
            static_cast<int>(layout.queryBox.w - (layout.textPadding * 2)),
            static_cast<int>(layout.queryBox.h)};
        renderer.pushClipRect(searchClipRect);
        renderer.drawRect(cursorX, layout.textY, 2, editorLayout.lineHeight, theme.cursor);
        renderer.clearClipRect();
    }
}

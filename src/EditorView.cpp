

#include "EditorView.h"
#include "Renderer.h"

void EditorView::render(Renderer &renderer, const Editor &editor, const EditorViewport& viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const EditorLayout &editorLayout)
{
    if (editor.isSearchActive())
    {
        renderSearchMatches(renderer, editor, viewport, layoutConfig);
    }
    renderLineNumbers(renderer, editor.getLineCount(), editor.getScrollOffsetY(), editor.getVisibleRows(), textLayout, layoutConfig);
    SDL_Rect clipRect{
        (int)layoutConfig.editorMarginLeft,
        0,
        (int)editorLayout.viewport.w - (int)layoutConfig.editorMarginLeft,
        (int)editorLayout.viewport.h};
    renderer.pushClipRect(clipRect);
    if (editor.getSelectionActive())
    {
        renderSelection(renderer, editor, viewport, layoutConfig);
    }

    renderCursor(renderer, editor, viewport, textLayout, layoutConfig);
    renderText(renderer, editor, viewport, textLayout, layoutConfig);

    renderer.clearClipRect();
}

void EditorView::renderLineNumbers(Renderer &renderer, uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows, const TextLayout &textLayout, const LayoutConfig &layoutConfig)
{
    const auto &theme = renderer.getTheme();
    uint32_t first = scrollOffsetY;
    uint32_t last = std::min(first + visibleRows, numLines);
    for (uint32_t i = first; i < last; ++i)
    {

        renderer.drawText(std::to_string(i + 1), layoutConfig.lineNumberWidth / 2 - textLayout.width(std::to_string(i + 1)) / 2, renderer.screenY(i, scrollOffsetY, layoutConfig.editorMarginTop), theme.lineNumbers);
    }
}

void EditorView::renderSelection(Renderer &renderer, const Editor &editor, const EditorViewport& viewport, const LayoutConfig &layoutConfig)
{
    Selection selection = editor.getSelection().normalized();

    if (selection.empty())
        return;

    const Position &start = selection.begin;
    const Position &end = selection.end;

    for (size_t row = start.row; row <= end.row; ++row)
    {
        if (row < editor.getScrollOffsetY())
            continue;
        if (row >= editor.getScrollOffsetY() + editor.getVisibleRows())
            break;

        int beginCol, endCol;
        if (row == start.row)
        {

            beginCol = start.col;
            // if only one line selected
            if (start.row == end.row)
            {
                endCol = end.col;
            }
            else
            {
                endCol = editor.getLineString(row).size();
            }
        }
        // in between line -> should be fully selected
        else if (row < end.row)
        {
            beginCol = 0;
            endCol = editor.getLineString(row).size();
        }
        else
        {
            beginCol = 0;
            endCol = end.col;
        }
        const std::string &line = editor.getLineString(row);
        renderer.renderHighlightedRange(line, row, beginCol, endCol - beginCol, editor.getScrollOffsetY(), viewport.scrollX(), layoutConfig);
    }
}

void EditorView::renderCursor(Renderer &renderer, const Editor &editor, const EditorViewport& viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig)
{
    const auto &layout = renderer.getSDL_Properties();
    const auto &theme = renderer.getTheme();
    Cursor cursor = editor.getCursor();
    std::string text = editor.getLineString(cursor.row);

    if (renderer.getCursorBlinker().visible())
    {
        int x = layoutConfig.editorMarginLeft + textLayout.columnToPixel(text, cursor.col) - viewport.scrollX();
        int y = renderer.screenY(cursor.row, editor.getScrollOffsetY(), layoutConfig.editorMarginTop);

        renderer.drawRect(x, y, 2, layout.lineHeight, theme.cursor);
    }
}

void EditorView::renderText(Renderer &renderer, const Editor &editor, const EditorViewport& viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig)
{
    auto &text = editor.getText();
    int visRows = editor.getVisibleRows();
    int first = editor.getScrollOffsetY();
    int last = std::min(
        first + visRows,
        (int)text.size());
    const auto &tokens = editor.getTokens();
    for (int i = first; i < last; ++i)
    {
        if (tokens.size() > 0)
        {
            renderer.drawTextTokenized(text[i], renderer.screenY(i, first, layoutConfig.editorMarginTop), tokens[i], viewport.scrollX(), layoutConfig);
        }
        else
        {
            renderer.drawText(textLayout.expandTabs(text[i]), layoutConfig.editorMarginLeft - viewport.scrollX(), renderer.screenY(i, first, layoutConfig.editorMarginTop));
        }
    }
}

void EditorView::renderSearchMatches(Renderer &renderer, const Editor &editor, const EditorViewport& viewport, const LayoutConfig &layoutConfig)
{
    if (editor.getSearch().getMatches().size() == 0)
    {
        return;
    }
    for (SearchMatch &match : editor.getSearch().getMatches())
    {
        const std::string &line = editor.getLineString(match.row);
        renderer.renderHighlightedRange(line, match.row, match.col, match.length, editor.getScrollOffsetY(), viewport.scrollX(), layoutConfig);
    }
}


#include "EditorView.h"

RenderColor EditorView::getColorFromTokenType(const Token &token) const
{
    switch (token.type)
    {
    case TokenType::OpenCurly:
    case TokenType::CloseCurly:
    case TokenType::OpenParen:
    case TokenType::CloseParen:
        return mLexerTheme.punctuation;
    case TokenType::Comment:
        return mLexerTheme.comment;
    case TokenType::String:
        return mLexerTheme.string;
    case TokenType::Keyword:
        return mLexerTheme.keyword;
    case TokenType::Preprocessor:
        return mLexerTheme.preprocessor;
    case TokenType::IncludeLib:
        return mLexerTheme.includeLib;
    default:
        return mLexerTheme.Default;
    }
}

void EditorView::renderHighlightedRange(RenderContext &renderContext, const TextLayout &textLayout, const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY, uint32_t scrollOffsetX, const LayoutConfig &layoutConfig)
{
    const auto &layout = renderContext.getWindowProperties();
    const auto &theme = renderContext.getTheme();
    std::string selectedText = textLayout.expandTabs(text.substr(col, length));
    int x = layoutConfig.editorMarginLeft + textLayout.columnToPixel(text, col) - scrollOffsetX;
    int y = screenY(layout, row, scrollOffsetY, layoutConfig.editorMarginTop);
    int w = textLayout.width(selectedText);
    renderContext.drawRect(x, y, w, layout.lineHeight, theme.selection);
}

int EditorView::screenY(const Window_Properties &layout, uint32_t row, uint32_t scrollOffset, uint32_t editorMarginTop) const
{
    return editorMarginTop + (row - scrollOffset) * layout.lineHeight;
}

void EditorView::render(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const EditorLayout &editorLayout, bool cursorVisible)
{
    if (editor.isSearchActive())
    {
        renderSearchMatches(renderContext, editor, viewport, textLayout, layoutConfig);
    }
    renderLineNumbers(renderContext, editor.getLineCount(), viewport.scrollY(), editor.getVisibleRows(), textLayout, layoutConfig);
    Rect clipRect{
        layoutConfig.editorMarginLeft,
        0,
        editorLayout.viewport.w - layoutConfig.editorMarginLeft,
        editorLayout.viewport.h};
    renderContext.pushClipRect(clipRect);
    if (editor.getSelectionActive())
    {
        renderSelection(renderContext, editor, viewport, textLayout, layoutConfig);
    }

    renderCursor(renderContext, editor, viewport, textLayout, layoutConfig, cursorVisible);
    renderText(renderContext, editor, viewport, textLayout, layoutConfig, editor.getEditorMode() == EditorMode::MLG);

    renderContext.clearClipRect();
}

void EditorView::renderLineNumbers(RenderContext &renderContext, uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows, const TextLayout &textLayout, const LayoutConfig &layoutConfig)
{
    const auto &layout = renderContext.getWindowProperties();
    const auto &theme = renderContext.getTheme();
    uint32_t first = scrollOffsetY;
    uint32_t last = std::min(first + visibleRows, numLines);
    for (uint32_t i = first; i < last; ++i)
    {

        renderContext.drawText(std::to_string(i + 1), layoutConfig.lineNumberWidth / 2 - textLayout.width(std::to_string(i + 1)) / 2, screenY(layout, i, scrollOffsetY, layoutConfig.editorMarginTop), theme.lineNumbers);
    }
}

void EditorView::renderSelection(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig)
{
    Selection selection = editor.getSelection().normalized();

    if (selection.empty())
        return;

    const Position &start = selection.begin;
    const Position &end = selection.end;

    for (size_t row = start.row; row <= end.row; ++row)
    {
        if (row < viewport.scrollY())
            continue;
        if (row >= viewport.scrollY() + editor.getVisibleRows())
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
        renderHighlightedRange(renderContext, textLayout, line, row, beginCol, endCol - beginCol, viewport.scrollY(), viewport.scrollX(), layoutConfig);
    }
}

void EditorView::renderCursor(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, bool cursorVisible)
{
    const auto &layout = renderContext.getWindowProperties();
    const auto &theme = renderContext.getTheme();
    Cursor cursor = editor.getCursor();
    std::string text = editor.getLineString(cursor.row);

    if (cursorVisible)
    {
        int x = layoutConfig.editorMarginLeft + textLayout.columnToPixel(text, cursor.col) - viewport.scrollX();
        int y = screenY(layout, cursor.row, viewport.scrollY(), layoutConfig.editorMarginTop);

        renderContext.drawRect(x, y, 2, layout.lineHeight, theme.cursor);
    }
}

void EditorView::renderText(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, bool mlg)
{
    auto &text = editor.getText();
    int visRows = editor.getVisibleRows();
    int first = viewport.scrollY();
    int last = std::min(
        first + visRows,
        (int)text.size());
    const auto &tokens = editor.getTokens();
    for (int i = first; i < last; ++i)
    {
        if (mlg)
        {
            renderContext.drawRainbowText(textLayout.expandTabs(text[i]), layoutConfig.editorMarginLeft - viewport.scrollX(), screenY(renderContext.getWindowProperties(), i, first, layoutConfig.editorMarginTop));
        }
        else if (tokens.size() > 0)
        {
            std::string expandedLine = textLayout.expandTabs(text[i]);
            int y = screenY(renderContext.getWindowProperties(), i, first, layoutConfig.editorMarginTop);
            for (const Token &token : tokens[i])
            {
                uint32_t virtualCol = textLayout.virtualColumn(text[i], token.col);
                std::string tokenText = expandedLine.substr(virtualCol, token.length);
                int xOffset = textLayout.width(std::string(virtualCol, ' '));
                int renderX = layoutConfig.editorMarginLeft - viewport.scrollX() + xOffset;
                renderContext.drawText(tokenText, renderX, y, getColorFromTokenType(token));
            }
        }
        else
        {
            renderContext.drawText(textLayout.expandTabs(text[i]), layoutConfig.editorMarginLeft - viewport.scrollX(), screenY(renderContext.getWindowProperties(), i, first, layoutConfig.editorMarginTop));
        }
    }
}

void EditorView::renderSearchMatches(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig)
{
    if (editor.getSearch().getMatches().size() == 0)
    {
        return;
    }
    for (const SearchMatch &match : editor.getSearch().getMatches())
    {
        const std::string &line = editor.getLineString(match.row);
        renderHighlightedRange(renderContext, textLayout, line, match.row, match.col, match.length, viewport.scrollY(), viewport.scrollX(), layoutConfig);
    }
}
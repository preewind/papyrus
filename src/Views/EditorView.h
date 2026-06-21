#pragma once

#include "Editor.h"
#include "EditorViewPort.h"
#include "RenderContext.h"
#include "TextLayout.h"
#include "LayoutManager.h"

class EditorView
{

public:
    void render(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const EditorLayout &editorLayout, bool cursorVisible);

private:
    RenderColor getColorFromTokenType(const Token &token) const;
    void renderHighlightedRange(RenderContext &renderContext, const TextLayout &textLayout, const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY, uint32_t scrollOffsetX, const LayoutConfig &layoutConfig);
    int screenY(const SDL_Properties &layout, uint32_t row, uint32_t scrollOffset, uint32_t editorMarginTop) const;
    void renderLineNumbers(RenderContext &renderContext, uint32_t numLines, uint32_t scrollOffsetY, uint32_t visibleRows, const TextLayout &textLayout, const LayoutConfig &layoutConfig);
    void renderSelection(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig);
    void renderCursor(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig, bool cursorVisible);
    void renderText(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig);
    void renderSearchMatches(RenderContext &renderContext, const Editor &editor, const EditorViewport &viewport, const TextLayout &textLayout, const LayoutConfig &layoutConfig);

    LexerTheme mLexerTheme;
};
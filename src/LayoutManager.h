#pragma once
#include "types.h"

struct LayoutConfig
{
    uint32_t editorMarginTop = 20;
    uint32_t editorMarginLeft = 40;
    uint32_t editorMarginRight = 50;

    uint32_t lineNumberWidth = 40;

    float terminalHeightRatio = 0.25f;
};

struct EditorLayout
{
    Rect viewport;
    Rect textArea;
    Rect lineNumberArea;
};

struct SearchLayout
{
    Rect queryBox;
    Rect matchBox;
    uint32_t textX;
    uint32_t textY;
    uint32_t matchBoxTextX;
    uint32_t matchBoxPadding = 10;
    uint32_t boxSpacing = 5;
    uint32_t textPadding = 5;
};

struct TerminalLayout
{
    Rect viewport;
    uint32_t marginTop = 10;
    uint32_t marginLeft = 10;
};


class LayoutManager{

public:
    void recalculate(uint32_t windowWidth, uint32_t windowHeight, uint32_t lineHeight, bool terminalVisible);

    const EditorLayout& getEditorLayout() const;
    const SearchLayout& getSearchLayout() const;
    const TerminalLayout& getTerminalLayout() const;

private:
    LayoutConfig mConfig;
    EditorLayout mEditor;
    SearchLayout mSearch;
    TerminalLayout mTerminal;
};
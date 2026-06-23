#pragma once
#include "types.h"

struct LayoutInput
{
    uint32_t windowWidth;
    uint32_t windowHeight;
    uint32_t lineHeight;
};

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

struct FileBrowserLayout
{
    Rect listViewport;
    uint32_t lineHeight = 0;
    uint32_t visibleRows = 0;

    uint32_t pathTextX = 0;
    uint32_t pathTextY = 0;
    uint32_t statusTextX = 0;
    uint32_t statusTextY = 0;

    uint32_t legendAnchorRightX = 0;
    uint32_t legendY = 0;
    uint32_t legendMarkerSize = 0;
};

class LayoutManager{

public:
    void update(const Window_Properties& input, bool terminalVisible);

    const LayoutConfig &getLayoutConfig() const;
    const EditorLayout& getEditorLayout() const;
    const SearchLayout& getSearchLayout() const;
    const TerminalLayout& getTerminalLayout() const;
    const FileBrowserLayout &getFileBrowserLayout() const;
    const LayoutInput& getLayoutInput() const;

private:
    LayoutConfig mConfig;
    EditorLayout mEditor;
    SearchLayout mSearch;
    TerminalLayout mTerminal;
    FileBrowserLayout mFileBrowser;
    LayoutInput mLayoutInput;
};
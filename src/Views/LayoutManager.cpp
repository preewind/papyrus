
#include <algorithm>

#include "LayoutManager.h"

void LayoutManager::update(const Window_Properties& input, bool terminalVisible)
{
    mLayoutInput.windowHeight = input.totalWindowHeight;
    mLayoutInput.windowWidth = input.totalWindowWidth;
    mLayoutInput.lineHeight = input.lineHeight;
    uint32_t editorViewPortHeight = terminalVisible ? input.totalWindowHeight * (1.0f-mConfig.terminalHeightRatio): input.totalWindowHeight;
    mEditor.viewport = {0, 0, input.totalWindowWidth, editorViewPortHeight};

    uint32_t terminalWindowHeight = mConfig.terminalHeightRatio * input.totalWindowHeight;
    uint32_t terminalWindowY = input.totalWindowHeight - terminalWindowHeight;
    mTerminal.viewport = {
        0, terminalWindowY, input.totalWindowWidth, terminalWindowHeight
    };
    uint32_t matchBoxWidth = 100;
    // search layout
    uint32_t searchQueryBoxX = mConfig.editorMarginLeft + input.totalWindowWidth / 2;
    uint32_t searchQueryBoxY = mConfig.editorMarginTop + input.lineHeight;
    uint32_t searchQueryBoxWidth = input.totalWindowWidth - searchQueryBoxX - mConfig.editorMarginRight - matchBoxWidth;
    uint32_t searchQueryHeight = 1.5 * input.lineHeight;
    mSearch.queryBox = {searchQueryBoxX, searchQueryBoxY, searchQueryBoxWidth, searchQueryHeight};
    

    uint32_t searchMatchBoxX = searchQueryBoxX + searchQueryBoxWidth + mSearch.boxSpacing;
    uint32_t searchMatchBoxY = searchQueryBoxY;
    uint32_t searchMatchBoxHeight = searchQueryHeight;
    mSearch.matchBox = {searchMatchBoxX, searchMatchBoxY, matchBoxWidth, searchMatchBoxHeight};
    
    mSearch.textX = searchQueryBoxX + mSearch.textPadding;
    mSearch.textY = searchQueryBoxY + (searchQueryHeight - input.lineHeight) / 2; // gives you a vertically centered text
    mSearch.matchBoxTextX =searchMatchBoxX + mSearch.textPadding;

    // file browser layout
    mFileBrowser.lineHeight = input.lineHeight;
    mFileBrowser.pathTextX = mConfig.editorMarginLeft;
    mFileBrowser.pathTextY = mConfig.editorMarginTop;
    mFileBrowser.statusTextX = mConfig.editorMarginLeft;
    mFileBrowser.statusTextY = mConfig.editorMarginTop + input.lineHeight;

    uint32_t fileListTop = mConfig.editorMarginTop + (input.lineHeight * 2);
    uint32_t listWidth = 0;
    if (input.totalWindowWidth > mConfig.editorMarginLeft + mConfig.editorMarginRight)
    {
        listWidth = input.totalWindowWidth - mConfig.editorMarginLeft - mConfig.editorMarginRight;
    }

    uint32_t listHeight = 0;
    if (input.totalWindowHeight > fileListTop)
    {
        listHeight = input.totalWindowHeight - fileListTop;
    }

    mFileBrowser.listViewport = {mConfig.editorMarginLeft, fileListTop, listWidth, listHeight};
    mFileBrowser.visibleRows = (input.lineHeight > 0) ? (listHeight / input.lineHeight) : 0;

    if (input.totalWindowWidth > mConfig.editorMarginRight)
    {
        mFileBrowser.legendAnchorRightX = input.totalWindowWidth - mConfig.editorMarginRight;
    }
    else
    {
        mFileBrowser.legendAnchorRightX = 0;
    }
    mFileBrowser.legendY = mConfig.editorMarginTop;
    mFileBrowser.legendMarkerSize = std::max(8u, input.lineHeight / 2);

}

const LayoutConfig &LayoutManager::getLayoutConfig() const
{
    return mConfig;
}

const EditorLayout &LayoutManager::getEditorLayout() const
{
    return mEditor;
}

const SearchLayout &LayoutManager::getSearchLayout() const
{
    return mSearch;
}

const TerminalLayout &LayoutManager::getTerminalLayout() const
{
    return mTerminal;
}

const FileBrowserLayout &LayoutManager::getFileBrowserLayout() const
{
    return mFileBrowser;
}

const LayoutInput &LayoutManager::getLayoutInput() const
{
    return mLayoutInput;
}

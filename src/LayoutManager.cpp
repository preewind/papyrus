

#include "LayoutManager.h"

void LayoutManager::update(const LayoutInput& input, bool terminalVisible)
{

    uint32_t editorViewPortHeight = terminalVisible ? input.windowHeight * (1.0f-mConfig.terminalHeightRatio): input.windowHeight;
    mEditor.viewport = {0, 0, input.windowWidth, editorViewPortHeight};

    uint32_t terminalWindowHeight = mConfig.terminalHeightRatio * input.windowHeight;
    uint32_t terminalWindowY = input.windowHeight - terminalWindowHeight;
    mTerminal.viewport = {
        0, terminalWindowY, input.windowWidth, terminalWindowHeight
    };
    uint32_t matchBoxWidth = 100;
    // search layout
    uint32_t searchQueryBoxX = mConfig.editorMarginLeft + input.windowWidth / 2;
    uint32_t searchQueryBoxY = mConfig.editorMarginTop + input.lineHeight;
    uint32_t searchQueryBoxWidth = input.windowWidth - searchQueryBoxX - mConfig.editorMarginRight - matchBoxWidth;
    uint32_t searchQueryHeight = 1.5 * input.lineHeight;
    mSearch.queryBox = {searchQueryBoxX, searchQueryBoxY, searchQueryBoxWidth, searchQueryHeight};
    

    uint32_t searchMatchBoxX = searchQueryBoxX + searchQueryBoxWidth + mSearch.boxSpacing;
    uint32_t searchMatchBoxY = searchQueryBoxY;
    uint32_t searchMatchBoxHeight = searchQueryHeight;
    mSearch.matchBox = {searchMatchBoxX, searchMatchBoxY, matchBoxWidth, searchMatchBoxHeight};
    
    mSearch.textX = searchQueryBoxX + mSearch.textPadding;
    mSearch.textY = searchQueryBoxY + (searchQueryHeight - input.lineHeight) / 2; // gives you a vertically centered text
    mSearch.matchBoxTextX =searchMatchBoxX + mSearch.textPadding;

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

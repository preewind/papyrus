

#include "LayoutManager.h"

void LayoutManager::recalculate(uint32_t windowWidth, uint32_t windowHeight, uint32_t lineHeight, bool terminalVisible)
{

    uint32_t editorViewPortHeight = terminalVisible ? windowHeight * (1.0f-mConfig.terminalHeightRatio): windowHeight;
    mEditor.viewport = {0, 0, windowWidth, editorViewPortHeight};

    uint32_t terminalWindowHeight = mConfig.terminalHeightRatio * windowHeight;
    uint32_t terminalWindowY = windowHeight - terminalWindowHeight;
    mTerminal.viewport = {
        0, terminalWindowY, windowWidth, terminalWindowHeight
    };
    uint32_t matchBoxWidth = 100; // TODO calc dynamically when factored out of searchView
    // search layout
    uint32_t searchQueryBoxX = mConfig.editorMarginLeft + windowWidth / 2;
    uint32_t searchQueryBoxY = mConfig.editorMarginTop + lineHeight;
    uint32_t searchQueryBoxWidth = windowWidth - searchQueryBoxX - mConfig.editorMarginRight - matchBoxWidth;
    uint32_t searchQueryHeight = 1.5 * lineHeight;
    mSearch.queryBox = {searchQueryBoxX, searchQueryBoxY, searchQueryBoxWidth, searchQueryHeight};
    

    uint32_t searchMatchBoxX = searchQueryBoxX + searchQueryBoxWidth + mSearch.boxSpacing;
    uint32_t searchMatchBoxY = searchQueryBoxY;
    uint32_t searchMatchBoxHeight = searchQueryHeight;
    mSearch.matchBox = {searchMatchBoxX, searchMatchBoxY, matchBoxWidth, searchMatchBoxHeight};
    
    mSearch.textX = searchQueryBoxX + mSearch.textPadding;
    mSearch.textY = searchQueryBoxY + (searchQueryHeight - lineHeight) / 2; // gives you a vertically centered text
    mSearch.matchBoxTextX =searchMatchBoxX + mSearch.textPadding;

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

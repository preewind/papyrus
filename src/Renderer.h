#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "types.h"
#include "theme.h"

class Cursor;
class Editor;
class Selection;
class FileBrowser;
class SearchSession;
class Terminal;

struct EditorLayout {
    uint16_t marginTop = 20;
    uint16_t marginLeft = 40;
    uint32_t marginRight = 50;
    uint16_t lineNumberAreaWidth = 40;
    uint16_t lineHeight = 0;
    uint16_t windowWidth = 0;
    uint16_t windowHeight = 0;
    uint16_t totalWindowHeight = 0;
};

struct TerminalLayout {
    uint32_t windowHeight = 0;
    uint32_t windowX = 0;
    uint32_t windowY = 0;
    uint32_t marginLeft = 10;
    uint32_t marginTop = 10;

};

struct SearchOverlayLayout{
    uint32_t queryX;
    uint32_t queryY;
    uint32_t queryWidth;
    uint32_t queryHeight;
    uint32_t matchBoxX;
    uint32_t matchBoxWidth = 100;
    uint32_t matchBoxPadding = 10;
    uint32_t textX;
    uint32_t textY;
    uint32_t matchBoxTextX;
    uint32_t textPadding = 5;
    uint32_t boxSpacing = 5;
};


class Renderer{

public:
    Renderer(SDL_Window* window);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void clear();
    uint32_t measureTextWidth(const std::string& text);
    int getLineHeight() const;
    const EditorLayout &getEditorLayout() const;
    std::string expandTabs(const std::string& text);
    SDL_Color getColorFromTokenType(const Token &token);
    void drawText(const std::string &text, int x, int y);
    void drawText(const std::string &text, int x, int y, SDL_Color color);
    void drawTextTokenized(const std::string& text, uint32_t y, const std::vector<Token> &tokens);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    void resetCursorBlink();
    void renderLineNumbers(uint32_t numLines, uint32_t offsetY, uint32_t visibleRows);
    void renderCursor(const Cursor &cursor, const std::string &text, uint32_t offsetY);  
    void renderText(const Editor &editor); 
    void renderSelection(const Editor &editor);
    void renderEditor(const Editor &editor);
    void renderTerminal(const Editor &editor);
    void renderTerminalCursor(const Terminal& terminal);
    void renderHighlightedRange(const std::string &text, uint32_t row, uint32_t col, uint32_t length,  uint32_t scrollOffsetY);
    void renderSearchOverlay(const SearchSession &session);
    void renderSearchCursor(const SearchSession &session);
    void renderSearchMatches(const SearchSession &session, const Editor &editor);
    void updateCursor();
    void updateEditor(Editor &editor);
    void updateFileBrowser(FileBrowser &browser);
    void renderFileBrowserSelection(FileBrowser &browser);
    const std::string fitTextToWidthFile(const std::string &text, std::string &extension);
    uint32_t getVirtualCol(const std::string &text, uint32_t rawCol);
    void present();

    void onResize(uint32_t w, uint32_t h);

    void setFontSize();
    void handlePlus(SDL_Keymod mod);
    void handleMinus(SDL_Keymod mod);

    void ensureCursorVisibleHorizontally(const Cursor& cursor,const std::string& line);
    void ensureCursorVisibleHorizontallySearch(uint32_t cursor,const std::string& line);

    int textX(const std::string &line, uint32_t col);
    int screenY(uint32_t row, uint32_t scrollOffset) const;
    int screenYBrowser(uint32_t row, uint32_t scrollOffset, uint32_t margin) const;
private: 
    Uint64 mLastBlink;
    bool mCursorVisible;
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;
    uint8_t mFontSize = 20;
    Theme mTheme;
    LexerTheme mLexerTheme;
    EditorLayout mLayout;
    SearchOverlayLayout mSearchLayout;
    TerminalLayout mTerminalLayout;
    int mScrollOffsetX = 0;
    int mScrollOffsetXSearch = 0;
};
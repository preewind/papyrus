#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "types.h"
#include "theme.h"
#include "CursorBlinker.h"
#include "TextLayout.h"
#include "EditorView.h"
#include "SearchView.h"
#include "TerminalView.h"
#include "FileBrowserView.h"
#include "LayoutManager.h"

class Cursor;
class Editor;
class Selection;
class FileBrowser;
class SearchSession;
class Terminal;

struct SDL_Properties
{
    uint32_t lineHeight = 0;
    uint32_t totalWindowWidth = 0;
    uint32_t totalWindowHeight = 0;
};

struct ScrollViewport
{
    int offsetX = 0;
    int visibleWidth = 0;

    void ensureVisible(int x, int padding = 0)
    {
        if (x < offsetX)
        {
            offsetX = x;
        }
        else if (x > offsetX + visibleWidth)
        {
            offsetX = x - visibleWidth + padding;
        }
    }
};

class Renderer
{

public:
    Renderer(SDL_Window *window);
    ~Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    void clear();
    int getLineHeight() const;
    const LayoutConfig &getLayoutConfig() const;
    const SDL_Properties &getSDL_Properties() const;
    const EditorLayout &getEditorLayout() const;
    const SearchLayout &getSearchLayout() const;
    const TerminalLayout &getTerminalLayout() const;
    const Theme &getTheme() const;
    const TextLayout &getTextLayout() const;
    const CursorBlinker &getCursorBlinker() const;
    uint32_t getScrollOffsetX() const;
    uint32_t getScrollOffsetXSearch() const;
    SDL_Color getColorFromTokenType(const Token &token);
    void drawText(const std::string &text, int x, int y);
    void drawText(const std::string &text, int x, int y, SDL_Color color);
    void drawTextTokenized(const std::string &text, uint32_t y, const std::vector<Token> &tokens);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    void drawRect(Rect rect, SDL_Color color);
    void pushClipRect(const SDL_Rect &rect);
    void clearClipRect();
    void renderEditor(const Editor &editor);
    void renderHighlightedRange(const std::string &text, uint32_t row, uint32_t col, uint32_t length, uint32_t scrollOffsetY);
    void updateLayout(const Editor& editor);
    void updateEditor(Editor &editor);
    void updateFileBrowser(FileBrowser &browser);
    const std::string fitTextToWidthFile(const std::string &text, std::string &extension);
    void present();

    void onResize(uint32_t w, uint32_t h);

    void setFontSize();
    void handlePlus(SDL_Keymod mod);
    void handleMinus(SDL_Keymod mod);

    void ensureCursorVisibleHorizontally(const Cursor &cursor, const std::string &line);
    void ensureCursorVisibleHorizontallySearch(uint32_t cursor, const std::string &line);

    int screenY(uint32_t row, uint32_t scrollOffset) const;
    int screenYBrowser(uint32_t row, uint32_t scrollOffset, uint32_t margin) const;

private:
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;
    TextLayout mTextLayout;
    EditorView mEditorView;
    SearchView mSearchView;
    TerminalView mTerminalView;
    FileBrowserView mFileBrowserView;
    uint8_t mFontSize = 20;
    ScrollViewport mEditorScrollPort;
    ScrollViewport mSearchScrollPort;
    CursorBlinker mCursorBlinker;
    Theme mTheme;
    LexerTheme mLexerTheme;
    LayoutManager mLayoutManager;
    SDL_Properties mLayout;
    int mScrollOffsetX = 0;
    int mScrollOffsetXSearch = 0;
};
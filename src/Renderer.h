#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Cursor;
class Editor;
class Selection;

struct EditorLayout {
    uint16_t marginTop = 20;
    uint16_t marginLeft = 40;
    uint16_t lineNumberAreaWidth = 40;
    uint16_t lineHeight = 0;
    uint16_t windowWidth = 0;
    uint16_t windowHeight = 0;
};


class Renderer{

public:
    Renderer() = delete;
    Renderer(SDL_Window* window);
    ~Renderer();

    void clear();
    int measureTextWidth(const std::string& text);
    int getLineHeight() const;
    const EditorLayout &getEditorLayout() const;
    std::string expandTabs(const std::string& text);
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    void resetCursorBlink();
    void renderLineNumbers(uint32_t numLines, uint32_t offsetY, uint32_t visibleRows);
    void renderCursor(const Cursor &cursor, const std::string &text, uint32_t offsetY);  
    void renderText(const Editor &editor); 
    void renderSelection(const Editor &editor);
    void renderEditor(const Editor &editor);
    void updateCursor();
    void update(Editor &editor);
    void present();

    void onResize(uint32_t w, uint32_t h);

    void ensureCursorVisibleHorizontally(const Cursor& cursor,const std::string& line);

    int textX(const std::string &line, uint32_t col);
    int screenY(uint32_t row, uint32_t scrollOffset) const;

private: 
    Uint64 mLastBlink;
    bool mCursorVisible;
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;
    EditorLayout mLayout;
    int mScrollOffsetX = 0;
};
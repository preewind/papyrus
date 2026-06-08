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
};


class Renderer{

public:
    Renderer() = delete;
    Renderer(SDL_Window* window);
    ~Renderer();

    void clear();
    int measureTextWidth(const std::string& text);
    int getLineHeight() const;
    std::string expandTabs(const std::string& text);
    void drawText(const std::string& text, int x, int y, SDL_Color color);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    void resetCursorBlink();
    void renderLineNumbers(int numLines);
    void renderCursor(const Cursor &cursor, const std::string &text);  
    void renderText(const std::vector<std::string> &text); 
    void renderSelection(const Editor &editor);
    void renderEditor(const Editor &editor);
    void updateCursor();
    void update(Editor &editor);
    void present();

private: 
    Uint64 mLastBlink;
    bool mCursorVisible;
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;
    EditorLayout mLayout;
};
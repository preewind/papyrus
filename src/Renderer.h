#pragma once

#include <string>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Cursor;
class Editor;

class Renderer{

public:
    Renderer() = delete;
    Renderer(SDL_Window* window);
    ~Renderer();

    void clear();
    int measureTextWidth(const std::string& text);
    int getLineHeight() const;
    std::string expandTabs(const std::string& text);
    void drawText(const std::string& text, int x, int y);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    void renderCursor(const Cursor &cursor, const std::string &text);  
    void renderText(const std::vector<std::string> &text); 
    void renderEditor(const Editor &editor);
    void updateCursor();
    void update(Editor &editor);
    void present();

private: 
    Uint64 mLastBlink;
    bool mCursorVisible;
    SDL_Renderer *mRenderer;
    TTF_Font *mFont;
};
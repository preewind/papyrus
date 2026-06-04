#pragma once

#include <string>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Renderer{

public:
    Renderer(SDL_Window* window);
    ~Renderer();

    void clear();
    int measureTextWidth(const std::string& text);
    int getLineHeight() const;
    void drawText(const std::string& text, int x, int y);
    void drawRect(int x, int y, int w, int h, SDL_Color color);
    void present();

private: 
    SDL_Renderer *renderer;
    TTF_Font *font;
};
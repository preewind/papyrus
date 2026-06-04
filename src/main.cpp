#include <iostream>
#include <vector>
#include <string>

#include <SDL3/SDL.h>

#include "Renderer.h"
#include "util.h"

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("papyrus", 800, 600, SDL_WINDOW_RESIZABLE);
    CSF(SDL_StartTextInput(window));

    Renderer sr{window};

    std::vector<std::string> text{""};


    // cursor position in characters
    int cursorX = 0;
    int cursorY = 0;
    Uint64 lastBlink = SDL_GetTicks();
    bool cursorVisible = true;
    int fontHeight = sr.getLineHeight();
    bool running = true;
    
    while (running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_TEXT_INPUT)
            {
                std::cout << "Key: " << event.text.text << "\n";
                if(text.size() < cursorY+1) text.push_back("");
                text[cursorY] += event.text.text;
                cursorX++;
            }
            if(event.type == SDL_EVENT_KEY_DOWN)
            {
                if(event.key.key == SDLK_BACKSPACE){
                    std::cout << "Backspace pressed!" << cursorX << "\n";
                    bool move = cursorX == 0;
                    if(!text[cursorY].empty())
                    {
                        text[cursorY].pop_back();
                        cursorX--;
                    } 
                    if(move && cursorY > 0)
                    {
                        std::cout << "test \n";
                        cursorY--;
                        cursorX = text[cursorY].size();
                    }
                }
                if(event.key.key == SDLK_RETURN){
                    cursorY++;
                    cursorX = 0;
                    if(text.size() < cursorY+1) text.push_back("");
                    std::cout << "Enter pressed! line:" <<  cursorY <<"\n";
                }
                if(event.key.key == SDLK_LEFT){
                    if(cursorX > 0)cursorX--;
                    std::cout << "LEFT pressed! cursor:" <<  cursorX <<"\n";
                }
                if(event.key.key == SDLK_RIGHT){
                    if(cursorX <= text[cursorY].size()-1)cursorX++;
                    std::cout << "RIGHT pressed! cursor:" <<  cursorX <<"\n";
                }


            }
            if(event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        Uint64 now = SDL_GetTicks();
        if (now - lastBlink > 500)
        {
            cursorVisible = !cursorVisible;
            lastBlink = now;
        }
        sr.clear();
        for(size_t i=0; i<text.size(); ++i){
            sr.drawText(text[i], 20, 20 + fontHeight*i);
        }

        
        if (cursorVisible)
        {

            int x = 20 + sr.measureTextWidth(text[cursorY].substr(0, cursorX));
            int y = 20 + cursorY * fontHeight;

            sr.drawRect(
                x,
                y,
                2,
                fontHeight,
                SDL_Color{255, 255, 255, 255}
            );
        }
        
        sr.present();
    }
    CSF(SDL_StopTextInput(window));
    SDL_DestroyWindow(window);
    SDL_Quit();
    

    return 0;
}
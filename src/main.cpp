#include <iostream>
#include <vector>
#include <string>

#include <SDL3/SDL.h>

#include "Renderer.h"
#include "Editor.h"
#include "util.h"

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("papyrus", 800, 600, SDL_WINDOW_RESIZABLE);
    CSF(SDL_StartTextInput(window));

    Renderer sr{window};

    Editor editor{};

    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_TEXT_INPUT)
            {
                editor.handleTextInput(event.text.text);
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_BACKSPACE)
                {
                    editor.handleBackSpace();
                }
                if (event.key.key == SDLK_RETURN)
                {
                    editor.handleReturn();
                }
                if (event.key.key == SDLK_LEFT)
                {
                    editor.handleLeft();
                }
                if (event.key.key == SDLK_RIGHT)
                {
                    editor.handleRight();
                }
                if (event.key.key == SDLK_UP)
                {
                    editor.handleUp();
                }
                if (event.key.key == SDLK_DOWN)
                {
                    editor.handleDown();
                }
                if (event.key.key == SDLK_TAB)
                {
                    editor.handleTab();
                }
            }
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }
        sr.update(editor);
    }
    CSF(SDL_StopTextInput(window));
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
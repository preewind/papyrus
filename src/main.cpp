#include <iostream>

#include <SDL3/SDL.h>


int main(void){
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("papyrus", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_StartTextInput(window);
    bool running = true;
    while (running)
    {
        SDL_Event event;

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_TEXT_INPUT){
                std::cout << "Key: " << event.text.text << "\n";
            }
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_StopTextInput(window);
    SDL_Quit();
    

    return 0;
}
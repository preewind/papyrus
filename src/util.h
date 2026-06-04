#pragma once

#include <stdexcept>
/*
    Macro to check the success of an SDL function, throws an error if not successfull
    csf = check sdl function
*/
#define CSF(x) \
    do { \
        if (!(x)) { \
            throw std::runtime_error( \
                std::string("SDL error in ") + __FILE__ + ":" + std::to_string(__LINE__) + \
                "\nCall " + #x + \
                "\nSDL_GetError: " + SDL_GetError() \
            ); \
        } \
    } while(0)

/*
    Macro to check the validity of an SDL pointer, throws an error if not null
    csp = check sdl pointer
*/
#define CSP(x) \
do { \
    if ((x) == nullptr) { \
        throw std::runtime_error( \
            std::string("SDL pointer error in ") + __FILE__ + ":" + std::to_string(__LINE__) + \
            "\nCall: " + #x + \
            "\nSDL_GetError: " + SDL_GetError() \
        ); \
    } \
} while(0)

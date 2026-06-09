#pragma once

#include <stdexcept>
/*
    Macro to check the success of an SDL function, throws an error if not successfull
    csf = check sdl function
*/
#define CSF(x)                                                                             \
    do                                                                                     \
    {                                                                                      \
        if (!(x))                                                                          \
        {                                                                                  \
            throw std::runtime_error(                                                      \
                std::string("SDL error in ") + __FILE__ + ":" + std::to_string(__LINE__) + \
                "\nCall " + #x +                                                           \
                "\nSDL_GetError: " + SDL_GetError());                                      \
        }                                                                                  \
    } while (0)

/*
    Macro to check the validity of an SDL pointer, throws an error if not null
    csp = check sdl pointer
*/
#define CSP(x)                                                                                     \
    do                                                                                             \
    {                                                                                              \
        if ((x) == nullptr)                                                                        \
        {                                                                                          \
            throw std::runtime_error(                                                              \
                std::string("SDL pointer error in ") + __FILE__ + ":" + std::to_string(__LINE__) + \
                "\nCall: " + #x +                                                                  \
                "\nSDL_GetError: " + SDL_GetError());                                              \
        }                                                                                          \
    } while (0)

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

inline std::vector<std::string> splitByNewline(const std::string &input)
{
    std::vector<std::string> result;

    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line, '\n'))
    {
        // pop trailing carriage return from Windows
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        result.push_back(line);
    }

    return result;
}

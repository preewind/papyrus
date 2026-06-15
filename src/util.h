#pragma once

#include <stdexcept>

#include "logger.h"
/*
    Macro to check the success of an SDL function, throws an error if not successfull
    csf = check sdl function
*/
#define CSF(x)                                                                                        \
    do                                                                                                \
    {                                                                                                 \
        if (!(x))                                                                                     \
        {                                                                                             \
            LOG_ERROR() << std::string("SDL error in ") + __FILE__ + ":" + std::to_string(__LINE__) + \
                               "\nCall " + #x +                                                       \
                               "\nSDL_GetError: " + SDL_GetError();                                   \
            exit(1);                                                                                  \
        }                                                                                             \
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

#include <SDL3/SDL_pixels.h>

inline SDL_Color hexToSDLColor(std::string hex) {
    if (!hex.empty() && hex[0] == '#') {
        hex = hex.substr(1);
    }

    if (hex.length() != 6 && hex.length() != 8) {
        // Fallback to white if the string format is invalid
        return SDL_Color{255, 255, 255, 255}; 
    }

    uint32_t colorValue = std::stoul(hex, nullptr, 16);

    SDL_Color color;
    if (hex.length() == 6) {
        color.r = (colorValue >> 16) & 0xFF;
        color.g = (colorValue >> 8)  & 0xFF;
        color.b = colorValue         & 0xFF;
        color.a = 255; 
    } else {
        color.r = (colorValue >> 24) & 0xFF;
        color.g = (colorValue >> 16) & 0xFF;
        color.b = (colorValue >> 8)  & 0xFF;
        color.a = colorValue         & 0xFF;
    }

    return color;
}

#include <algorithm>
#include <cctype>
inline std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();
    
    return (start < end) ? std::string(start, end) : "";
}

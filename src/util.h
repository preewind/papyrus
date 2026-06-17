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

    size_t start = 0;
    size_t end = input.find('\n');

    while (end != std::string::npos)
    {
        std::string line = input.substr(start, end - start);
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        result.push_back(line);
        
        start = end + 1;
        end = input.find('\n', start);
    }

    // push the remaining part of the string after the last newline
    std::string trailingLine = input.substr(start);
    if (!trailingLine.empty() && trailingLine.back() == '\r')
    {
        trailingLine.pop_back();
    }
    result.push_back(trailingLine);

    return result;
}

#include <SDL3/SDL_pixels.h>

inline SDL_Color hexToSDLColor(std::string hex)
{
    if (!hex.empty() && hex[0] == '#')
    {
        hex = hex.substr(1);
    }

    if (hex.length() != 6 && hex.length() != 8)
    {
        // Fallback to white if the string format is invalid
        return SDL_Color{255, 255, 255, 255};
    }

    uint32_t colorValue = std::stoul(hex, nullptr, 16);

    SDL_Color color;
    if (hex.length() == 6)
    {
        color.r = (colorValue >> 16) & 0xFF;
        color.g = (colorValue >> 8) & 0xFF;
        color.b = colorValue & 0xFF;
        color.a = 255;
    }
    else
    {
        color.r = (colorValue >> 24) & 0xFF;
        color.g = (colorValue >> 16) & 0xFF;
        color.b = (colorValue >> 8) & 0xFF;
        color.a = colorValue & 0xFF;
    }

    return color;
}

#include <algorithm>
#include <cctype>
inline std::string trim(const std::string &str)
{
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch)
                                  { return std::isspace(ch); });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch)
                                { return std::isspace(ch); })
                   .base();

    return (start < end) ? std::string(start, end) : "";
}

inline Range findWordLeftOfIndex(const std::string_view &str)
{
    if (str.empty())
        return {0, 0};

    size_t end = str.find_last_not_of(" \t\n\v\f\r");
    if (end == std::string::npos)
    {
        return {0, 0};
    }
    size_t start = str.find_last_of(" \t\n\v\f\r", end);

    if (start == std::string::npos)
    {
        start = 0;
    }
    else
    {
        start += 1;
    }

    return {start, end + 1};
}

inline Range findWordRightOfIndex(const std::string_view &str)
{
    if (str.empty())
        return {0, 0};
    size_t start = str.find_first_not_of(" \t\n\v\f\r");
    
    if (start == std::string::npos)
    {
        return {0, 0};
    }
    size_t end = str.find_first_of(" \t\n\v\f\r", start);
    if (end == std::string::npos)
    {
        end = str.length();
    }
    return {start, end};
}

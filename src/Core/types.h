#pragma once

#include <stddef.h>
#include <stdint.h>
#include <iostream>
#include <vector>

#include "logger.h"

enum class Screen
{
    Editor,
    FileBrowser,
    Screensaver
};

struct SearchMatch
{
    uint32_t row;
    uint32_t col;
    uint32_t length;
};

struct Range
{
    size_t start;
    size_t end;
};

struct Position
{
    size_t row, col;

    bool operator==(const Position &other) const
    {
        return row == other.row && col == other.col;
    }
    bool operator!=(const Position &other) const
    {
        return !(*this == other);
    }
    bool operator<(const Position &other) const
    {
        if (row != other.row)
        {
            return row < other.row;
        }

        return col < other.col;
    }
    bool operator>(const Position &other) const
    {
        return other < *this;
    }
    friend std::ostream &operator<<(std::ostream &os, const Position &pos)
    {
        os << "(" << pos.row << ", " << pos.col << ")";
        return os;
    }
};

struct Cursor : Position
{
    Cursor &operator=(const Position &other)
    {
        Position::operator=(other);
        return *this;
    }
};

struct Selection
{
    Position begin;
    Position end;

    bool empty() const
    {
        return begin == end;
    }

    Selection normalized() const
    {
        if (begin < end)
        {
            return *this;
        }
        return Selection{
            .begin = end,
            .end = begin};
    }
};

enum class TokenType
{
    Normal,
    OpenParen,
    CloseParen,
    OpenCurly,
    CloseCurly,
    Keyword,
    String,
    Number,
    Comment,
    Preprocessor,
    IncludeLib,
    Unknown
};

inline std::ostream &operator<<(std::ostream &os, const TokenType &type)
{
    switch (type)
    {
    case TokenType::Normal:
        os << "Normal";
        break;
    case TokenType::OpenParen:
        os << "OpenParen";
        break;
    case TokenType::CloseParen:
        os << "CloseParen";
        break;
    case TokenType::OpenCurly:
        os << "OpenCurly";
        break;
    case TokenType::CloseCurly:
        os << "CloseCurly";
        break;
    case TokenType::Keyword:
        os << "Keyword";
        break;
    case TokenType::String:
        os << "String";
        break;
    case TokenType::Number:
        os << "Number";
        break;
    case TokenType::Comment:
        os << "Comment";
        break;
    case TokenType::Preprocessor:
        os << "Preprocessor";
        break;
    case TokenType::IncludeLib:
        os << "IncludeLib";
        break;
    case TokenType::Unknown:
        os << "Unknown";
        break;
    default:
        LOG_ERROR() << "Dont forget!!!";
        break;
    }
    return os;
}

struct Token
{
    uint32_t col;
    uint32_t length;
    TokenType type;
};

enum class Language
{
    PlainText,
    Cpp
};

struct CommandResult
{
    bool success;
    std::string message;
};

enum class TerminalInputType
{
    Filename,
    Confirmation,
    TextInput,
    SelectFromList
};

struct TerminalInputRequest
{
    TerminalInputType type;
    std::string prompt;
    std::string defaultValue;
    std::vector<std::string> options; // For SelectFromList type
};

struct TerminalInputResponse
{
    bool success;
    std::string userInput;
    size_t selectedIndex = 0; // For SelectFromList type
};

struct Rect
{
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t w = 0;
    uint32_t h = 0;
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) : x(x), y(y), w(w), h(h) {}
};

struct Window_Properties
{
    uint32_t lineHeight = 0;
    uint32_t totalWindowWidth = 0;
    uint32_t totalWindowHeight = 0;
};
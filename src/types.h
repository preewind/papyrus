#pragma once

#include <stddef.h>
#include <stdint.h>
#include <iostream>

enum class Screen
{
    Editor,
    FileBrowser,
    Settings,
    Terminal
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
        throw(std::runtime_error("Dont forget!!!"));
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

enum class CommandRequestType
{
    OpenFile,
    SaveFile,
    Quit,
    ChangeLanguage,
    Error
};

struct CommandRequest
{
    CommandRequestType type;
    std::string request;
};

struct CommandResult
{
    bool success;
    std::string message;
};
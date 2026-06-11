#pragma once

#include <stddef.h>
#include <stdint.h>
#include <iostream>

enum class Screen {
    Editor,
    FileBrowser,
    Settings,
    Terminal
};

struct SearchMatch {
    uint32_t row;
    uint32_t col;
    uint32_t length;
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
    void operator=(const Position &other)
    {
        row = other.row;
        col = other.col;
    }
    friend std::ostream &operator<<(std::ostream &os, const Position &pos)
    {
        os << "(" << pos.row << ", " << pos.col << ")";
        return os;
    }
};

struct Cursor : Position
{
    void operator=(const Position &other)
    {
        row = other.row;
        col = other.col;
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
#include <stdexcept>
#include <string>
#include <iostream>

#include "TextBuffer.h"
#include "logger.h"
#include "util.h"

TextBuffer::TextBuffer(const std::string &text)
{
    mLines.push_back(text);
}

TextBuffer::TextBuffer(const std::vector<std::string> &buffer)
{
    mLines = buffer;
}

void TextBuffer::insert(size_t row, size_t col, const std::string &text)
{
    if (row == 0 && mLines.size() == 0)
    {
        mLines.resize(1);
    }
    else if (mLines.size() <= row)
    {
        mLines.resize(row + 1);
    }
    mLines[row].insert(col, text);
}

/**
 * @brief Inserts text at a coordinate, automatically splitting it into multiple lines if it contains '\n'.
 * * This is the ultimate insertion tool. It safely handles regular single-char typing,
 * carriage returns (\r\n), and pasting huge blocks of multi-line text.
 * * @param row The starting line index.
 * @param col The starting column character index.
 * @param text The string to inject (can contain newlines).
 * @return Position The exact coordinates where the cursor should land after insertion.
 */
Position TextBuffer::insertFormatted(size_t row, size_t col, const std::string &text)
{
    if (text.empty())
    {
        return {row, col};
    }
    std::vector<std::string> lines = splitByNewline(text);

    if (mLines.empty() || row >= mLines.size())
    {
        mLines.resize(row + 1);
    }

    std::string prefix = mLines[row].substr(0, col);
    std::string suffix = mLines[row].substr(col);

    // insert one line
    if (lines.size() == 1)
    {
        mLines[row] = prefix + lines.front() + suffix;
        return {row, col + lines[0].size()};
    }

    // first line in multiline case
    mLines[row] = prefix + lines.front();

    // lines in between
    for (size_t i = 1; i < lines.size(); ++i)
    {
        insertLine(row + i, lines[i]);
    }

    // last line
    mLines[row + lines.size() - 1] += suffix;

    return {row + lines.size() - 1, lines[lines.size() - 1].size()};
}

void TextBuffer::insertLine(size_t row, const std::string &text)
{
    mLines.insert(mLines.begin() + row, text);
}

void TextBuffer::addLine(const std::string &text)
{
    mLines.push_back(text);
}

void TextBuffer::erase(size_t row, size_t col)
{
    if (row >= mLines.size())
        return;
    if (col >= mLines[row].size())
        return;

    if (!mLines[row].empty())
    {
        mLines[row].erase(col, 1);
    }
}

void TextBuffer::eraseRange(size_t row, size_t begin_col, size_t end_col)
{
    if (row >= mLines.size())
        return;

    if (end_col > mLines[row].size())
        return;
    if (begin_col > mLines[row].size())
        return;
    if (end_col < begin_col)
        return;

    if (!mLines[row].empty())
    {
        mLines[row].erase(begin_col, end_col - begin_col);
    }
}

void TextBuffer::eraseRange(size_t row, Range range)
{
    eraseRange(row, range.start, range.end);
}

void TextBuffer::eraseRange(Position pos, uint32_t length)
{
    eraseRange(pos.row, pos.col, pos.col + length);
}

void TextBuffer::eraseRangeMultiRow(size_t begin_row, size_t begin_col, size_t end_row, size_t end_col)
{
    if (begin_row >= mLines.size() || end_row >= mLines.size() || begin_row > end_row)
    {
        return;
    }

    if (begin_row == end_row)
    {
        eraseRange(begin_row, begin_col, end_col);
        return;
    }

    std::string finalLineSuffix = "";
    if (end_col < mLines[end_row].size())
    {
        finalLineSuffix = mLines[end_row].substr(end_col);
    }

    // delete everything after begin col in first row
    if (begin_col <= mLines[begin_row].size())
    {
        mLines[begin_row].resize(begin_col);
    }

    mLines[begin_row] += finalLineSuffix;

    // remove lines in between
    mLines.erase(mLines.begin() + begin_row + 1, mLines.begin() + end_row + 1);
}

void TextBuffer::eraseRangeMultiRow(const Selection &selection)
{
    eraseRangeMultiRow(selection.begin.row, selection.begin.col, selection.end.row, selection.end.col);
}

/**
 * @brief Deletes a continuous block of text starting from a position, spanning across multiple lines if needed.
 * * It counts characters sequentially. If the length to delete is longer than the current line,
 * it safely jumps down to the next row, treats the line-break as 1 character, and keeps deleting.
 * * @param start The coordinate where the deletion begins.
 * @param length How many total characters to delete.
 */
void TextBuffer::eraseRangeSmart(Position start, uint32_t length)
{
    if (length == 0)
        return;

    size_t curRow = start.row;
    size_t curCol = start.col;

    size_t endRow = curRow;
    size_t endCol = curCol;

    // advance coordinates by 'length' characters to find the end boundary
    while (length > 0 && endRow < mLines.size())
    {
        size_t availableInLine = mLines[endRow].size() - endCol;

        if (length <= availableInLine)
        {
            endCol += length;
            length = 0;
        }
        else
        {
            // Consume the remaining characters on this line plus the implicit newline '\n'
            length -= (availableInLine + 1);
            endRow++;
            endCol = 0;
        }
    }
    eraseRangeMultiRow(curRow, curCol, endRow, endCol);
}

void TextBuffer::clear()
{
    mLines.clear();
}

void TextBuffer::splitLine(size_t row, size_t col)
{
    if (col > mLines[row].size())
        return;

    std::string secondPart = mLines[row].substr(col);
    mLines[row].resize(col);
    mLines.insert(mLines.begin() + row + 1, std::move(secondPart));
}

void TextBuffer::splitLine(const Cursor &cursor)
{
    splitLine(cursor.row, cursor.col);
}

void TextBuffer::mergeWithNext(size_t row)
{
    if (row >= mLines.size() || row + 1 >= mLines.size())
        return;
    mLines[row] += std::move(mLines[row + 1]);
    mLines.erase(mLines.begin() + row + 1);
}

void TextBuffer::setLines(const std::vector<std::string> &lines)
{
    mLines.clear();
    mLines = lines;
}

const std::string &TextBuffer::getLine(size_t row) const
{
    if (row < mLines.size())
    {
        return mLines[row];
    }
    throw std::runtime_error("Line doesn't exist!");
}

size_t TextBuffer::getLineSize(size_t row) const
{
    if (row < mLines.size())
    {
        return mLines[row].size();
    }
    return 0;
}

size_t TextBuffer::getLineCount() const
{
    return mLines.size();
}

const std::vector<std::string> &TextBuffer::getText() const
{
    return mLines;
}

std::string TextBuffer::getTextSlice(Position &start, Position &end) const
{
    std::string result = "";
    for (size_t row = start.row; row <= end.row; row++)
    {

        int beginCol, endCol;
        if (row == start.row)
        {

            beginCol = start.col;
            // if only one line selected
            if (start.row == end.row)
            {
                endCol = end.col;
            }
            else
            {
                endCol = getLineSize(row);
            }
        }
        // in between line -> should be fully selected
        else if (row < end.row)
        {
            beginCol = 0;
            endCol = getLineSize(row);
        }
        else
        {
            beginCol = 0;
            endCol = end.col;
        }
        const std::string &line = getLine(row);
        result += line.substr(beginCol, endCol - beginCol);
        if (row < end.row)
        {
            result += "\n";
        }
    }
    return result;
}

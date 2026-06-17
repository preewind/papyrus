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

/*
    Inserts the passed text into the line buffer and splits at new lines.
    It returns the Position the Cursor is after inserting.
*/
Position TextBuffer::insertFormatted(size_t row, size_t col, const std::string &text)
{
    if (text.empty())
    {
        return {row, col};
    }
    std::vector<std::string> lines = splitByNewline(text);
    size_t newSize = row + lines.size();

    if (mLines.size() < newSize)
    {
        mLines.resize(newSize);
    }

    std::string prefix = mLines[row].substr(0, col);
    std::string suffix = mLines[row].substr(col);

    // insert one line
    if (lines.size() == 1)
    {
        mLines[row] = prefix + lines.front() + suffix;
        return {
            row,
            col + lines[0].size()};
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
    if (begin_row >= mLines.size() || end_row >= mLines.size() || begin_row > end_row) {
        return;
    }

    if(begin_row == end_row){
        eraseRange(begin_row, begin_col, end_col);
        return;
    }

    std::string finalLineSuffix = "";
    if (end_col < mLines[end_row].size()) {
        finalLineSuffix = mLines[end_row].substr(end_col);
    }

    // delete everything after begin col in first row
    if (begin_col <= mLines[begin_row].size()) {
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

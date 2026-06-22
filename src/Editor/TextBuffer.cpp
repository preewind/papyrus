#include <algorithm>
#include <stdexcept>
#include <string>

#include "TextBuffer.h"
#include "logger.h"
#include "util.h"

TextBuffer::TextBuffer(const std::vector<std::string> &buffer)
{
    mLines = buffer;
    if(mLines.empty()){
        mLines.push_back("");
    }
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

    const size_t safeCol = std::min(col, mLines[row].size());
    std::string prefix = mLines[row].substr(0, safeCol);
    std::string suffix = mLines[row].substr(safeCol);

    // insert one line
    if (lines.size() == 1)
    {
        mLines[row] = prefix + lines.front() + suffix;
        return {row, safeCol + lines[0].size()};
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
    const size_t safeRow = std::min(row, mLines.size());
    mLines.insert(mLines.begin() + safeRow, text);
}

void TextBuffer::addLine(const std::string &text)
{
    mLines.push_back(text);
}

void TextBuffer::eraseRange(size_t row, size_t begin_col, size_t end_col)
{
    if (row >= mLines.size())
    {
        LOG_WARN() << "eraseRange: row out of bounds (row=" << row << ", lineCount=" << mLines.size() << ")";
        return;
    }

    const size_t lineSize = mLines[row].size();
    if (begin_col > lineSize || end_col > lineSize || end_col < begin_col)
    {
        LOG_WARN() << "eraseRange: invalid column range (row=" << row << ", begin=" << begin_col
                   << ", end=" << end_col << ", lineSize=" << lineSize << ")";
        return;
    }

    if (begin_col == end_col)
        return;

    mLines[row].erase(begin_col, end_col - begin_col);
}

void TextBuffer::eraseRangeMultiRow(size_t begin_row, size_t begin_col, size_t end_row, size_t end_col)
{
    if (begin_row >= mLines.size() || end_row >= mLines.size() || begin_row > end_row)
    {
        LOG_WARN() << "eraseRangeMultiRow: invalid row range (beginRow=" << begin_row
                   << ", endRow=" << end_row << ", lineCount=" << mLines.size() << ")";
        return;
    }

    const size_t beginLineSize = mLines[begin_row].size();
    const size_t endLineSize = mLines[end_row].size();
    if (begin_col > beginLineSize || end_col > endLineSize)
    {
        LOG_WARN() << "eraseRangeMultiRow: invalid column range (beginRow=" << begin_row
                   << ", beginCol=" << begin_col << ", beginLineSize=" << beginLineSize
                   << ", endRow=" << end_row << ", endCol=" << end_col
                   << ", endLineSize=" << endLineSize << ")";
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

    if (start.row >= mLines.size())
        return;

    size_t curRow = start.row;
    size_t curCol = std::min(start.col, mLines[curRow].size());

    size_t endRow = curRow;
    size_t endCol = curCol;

    // advance coordinates by 'length' characters to find the end boundary
    while (length > 0 && endRow < mLines.size())
    {
        const size_t lineSize = mLines[endRow].size();
        if (endCol > lineSize)
        {
            endCol = lineSize;
        }

        size_t availableInLine = lineSize - endCol;

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

    if (endRow >= mLines.size())
    {
        endRow = mLines.size() - 1;
        endCol = mLines[endRow].size();
    }

    eraseRangeMultiRow(curRow, curCol, endRow, endCol);
}

void TextBuffer::setLines(const std::vector<std::string> &lines)
{
    mLines.clear();
    mLines = lines;
    if(mLines.empty()){
        mLines.push_back("");
    }
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

std::string TextBuffer::getTextSlice(const Position &start, const Position &end) const
{
    if (start > end)
    {
        return "";
    }

    if (start.row >= mLines.size() || end.row >= mLines.size())
    {
        return "";
    }

    std::string result = "";
    for (size_t row = start.row; row <= end.row; row++)
    {
        const std::string &line = mLines[row];
        const size_t lineSize = line.size();

        size_t beginCol = 0;
        size_t endCol = lineSize;

        if (row == start.row)
        {
            beginCol = std::min(start.col, lineSize);
            // if only one line selected
            if (start.row == end.row)
            {
                endCol = std::min(end.col, lineSize);
            }
        }
        // in between line -> should be fully selected
        else if (row < end.row)
        {
            beginCol = 0;
            endCol = lineSize;
        }
        else
        {
            beginCol = 0;
            endCol = std::min(end.col, lineSize);
        }

        if (endCol < beginCol)
        {
            return "";
        }

        result += line.substr(beginCol, endCol - beginCol);
        if (row < end.row)
        {
            result += "\n";
        }
    }
    return result;
}

#pragma once

#include <string>
#include <vector>
#include "types.h"

class TextBuffer
{

public:
    TextBuffer() = default;
    TextBuffer(const std::string& text);
    TextBuffer(const std::vector<std::string> &buffer);
    //~TextBuffer();

    void insert(size_t row, size_t col, const std::string &text);
    Position insertFormatted(size_t row, size_t col, const std::string &text);
    void insertLine(size_t row, const std::string &text);
    void addLine(const std::string& text);
    void erase(size_t row, size_t col);
    void eraseRange(size_t row, size_t begin_col, size_t end_col);
    void eraseRange(size_t row, Range range);
    void eraseRangeMultiRow(size_t begin_row, size_t begin_col, size_t end_row, size_t end_col);
    void eraseRangeMultiRow(const Selection& selection);
    void clear();
    void splitLine(size_t row, size_t col);
    void splitLine(const Cursor& cursor);
    void mergeWithNext(size_t row);

    void setLines(const std::vector<std::string> &lines);

    const std::string &getLine(size_t row) const;
    size_t getLineSize(size_t row) const;
    size_t getLineCount() const;
    const std::vector<std::string> &getText() const;
    std::string getTextSlice(Position &start, Position &end) const;

private:
    std::vector<std::string> mLines{""};
};
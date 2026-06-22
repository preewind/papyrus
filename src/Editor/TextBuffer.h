#pragma once

#include <string>
#include <vector>
#include "types.h"

class TextBuffer
{

public:
    TextBuffer() = default;
    TextBuffer(const std::vector<std::string> &buffer);
  
    Position insertFormatted(size_t row, size_t col, const std::string &text);
    void insertLine(size_t row, const std::string &text);
    void addLine(const std::string &text);
    void eraseRange(size_t row, size_t begin_col, size_t end_col);
    void eraseRangeMultiRow(size_t begin_row, size_t begin_col, size_t end_row, size_t end_col);
    void eraseRangeSmart(Position start, uint32_t length);

    void setLines(const std::vector<std::string> &lines);

    const std::string &getLine(size_t row) const;
    size_t getLineSize(size_t row) const;
    size_t getLineCount() const;
    const std::vector<std::string> &getText() const;
    std::string getTextSlice(const Position &start, const Position &end) const;

private:
    std::vector<std::string> mLines{""};
};
#pragma once

#include <string>
#include <vector>
#include "types.h"

class TextBuffer {

public:
    //TextBuffer();
    //~TextBuffer();

    void insert(size_t row, size_t col, const std::string &text);
    Position insertFormatted(size_t row, size_t col, const std::string &text);
    void insertLine(size_t row, const std::string& text);
    void erase(size_t row, size_t col);
    void eraseRange(size_t row, size_t begin_col, size_t end_col);
    void splitLine(size_t row, size_t col);
    void mergeWithNext(size_t row);

    void setLines(const std::vector<std::string>& lines);

    const std::string& getLine(size_t row) const;
    size_t getLineSize(size_t row) const;
    size_t getLineCount() const;
    const std::vector<std::string>& getText() const;
    std::string getTextSlice(size_t start_row, size_t start_col, size_t end_row, size_t end_col) const;



private:
    std::vector<std::string> mLines{""};

};
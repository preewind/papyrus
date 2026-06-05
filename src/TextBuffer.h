#pragma once

#include <string>
#include <vector>

class TextBuffer {

public:
    //TextBuffer();
    //~TextBuffer();

    void insert(size_t row, size_t col, const std::string &text);
    void erase(size_t row, size_t col);
    void splitLine(size_t row, size_t col);
    void mergeWithNext(size_t row);
    const std::string& getLine(size_t row) const;
    size_t getLineCount() const;
    const std::vector<std::string>& getText() const;



private:
    std::vector<std::string> mLines{""};

};
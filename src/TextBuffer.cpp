#include <stdexcept>
#include <string>
#include <iostream>

#include "TextBuffer.h"

void TextBuffer::insert(size_t row, size_t col, const std::string &text)
{
    if (mLines.size() <= row)
    {
        mLines.resize(row+1);
    }
    mLines[row].insert(col, text);
}

void TextBuffer::erase(size_t row, size_t col)
{
    if(row >= mLines.size()) return;
    if(col >= mLines[row].size()) return;
    
    if (!mLines[row].empty())
    {
        mLines[row].erase(col, 1);
    }
}

void TextBuffer::splitLine(size_t row, size_t col)
{
    if(col > mLines[row].size()) return;

    std::string secondPart = mLines[row].substr(col);
    mLines[row].resize(col);
    mLines.insert(mLines.begin()+row+1, std::move(secondPart));
}

void TextBuffer::mergeWithNext(size_t row)
{
    if(row >= mLines.size() || row+1 >= mLines.size()) return;
    mLines[row] += std::move(mLines[row+1]);
    mLines.erase(mLines.begin() + row+1);
}

const std::string &TextBuffer::getLine(size_t row) const
{
    if (row < mLines.size())
    {
        return mLines[row];
    }
    throw std::runtime_error("Line doesn't exist!");
}

size_t TextBuffer::getLineCount() const
{
    return mLines.size();
}

const std::vector<std::string> &TextBuffer::getText() const
{
    return mLines;
}

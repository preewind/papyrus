#include "Editor.h"

Editor::Editor()
{
    mCursor = Cursor{0, 0};
}

Editor::~Editor()
{
}

void Editor::handleTextInput(const std::string &text)
{
    mBuffer.insert(mCursor.row, mCursor.col, text);
    mCursor.col+= text.size();
}

void Editor::handleBackSpace()
{
    if (mCursor.col > 0)
    {
        mBuffer.erase(mCursor.row, mCursor.col-1);
        mCursor.col--;
    }
    else if(mCursor.col == 0 && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mBuffer.getLine(mCursor.row).size();
        mBuffer.mergeWithNext(mCursor.row);
    }
    
}

void Editor::handleReturn()
{
    mBuffer.splitLine(mCursor.row, mCursor.col);
    mCursor.col = 0;
    mCursor.row++;
}

void Editor::handleLeft()
{
    if (mCursor.col > 0)
    {
        mCursor.col--;
    }
    else if (mCursor.col == 0 && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mBuffer.getLine(mCursor.row).size();
    }
}

void Editor::handleRight()
{
    if (mCursor.col < mBuffer.getLine(mCursor.row).size())
    {
        mCursor.col++;
    }
    else if (mCursor.col == mBuffer.getLine(mCursor.row).size() && mCursor.row < mBuffer.getLineCount() - 1)
    {
        mCursor.row++;
        mCursor.col = 0;
    }
}

Cursor Editor::getCursor() const
{
    return mCursor;
}

const std::string &Editor::getLineString(int i) const
{
    return mBuffer.getLine(i);
}

const std::vector<std::string> &Editor::getText() const
{
    return mBuffer.getText();
}

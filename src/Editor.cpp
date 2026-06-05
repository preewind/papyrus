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
    mCursor.col += text.size();
}

void Editor::handleBackSpace()
{
    if (mCursor.col > 0)
    {
        mBuffer.erase(mCursor.row, mCursor.col - 1);
        mCursor.col--;
    }
    else if (mCursor.col == 0 && mCursor.row > 0)
    {
        mCursor.row--;
        mCursor.col = mBuffer.getLineSize(mCursor.row);
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
        mCursor.col = mBuffer.getLineSize(mCursor.row);
    }
}

void Editor::handleRight()
{
    if (mCursor.col < mBuffer.getLineSize(mCursor.row))
    {
        mCursor.col++;
    }
    else if (mCursor.col == mBuffer.getLineSize(mCursor.row) && mCursor.row < mBuffer.getLineCount() - 1)
    {
        mCursor.row++;
        mCursor.col = 0;
    }
}

void Editor::handleUp()
{
    if (mCursor.row > 0)
    {
        mCursor.row--;
        bool isUpperShorter = mBuffer.getLineSize(mCursor.row) < mBuffer.getLineSize(mCursor.row+1);
        bool colGreaterThanLineAbove = mCursor.col >=  mBuffer.getLineSize(mCursor.row);
        if (isUpperShorter && colGreaterThanLineAbove)
        {
            mCursor.col = mBuffer.getLineSize(mCursor.row);
        }
    }
}

void Editor::handleDown()
{
    if (mCursor.row < mBuffer.getLineCount()-1)
    {
        mCursor.row++;
        bool isUpperLonger = mBuffer.getLineSize(mCursor.row-1) > mBuffer.getLineSize(mCursor.row);
        bool colGreaterThanLineBelow = mCursor.col >=  mBuffer.getLineSize(mCursor.row);
        if (isUpperLonger && colGreaterThanLineBelow)
        {
            mCursor.col = mBuffer.getLineSize(mCursor.row);
        }
    }
}

void Editor::handleTab()
{
    handleTextInput("\t");
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

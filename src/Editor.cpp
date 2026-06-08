#include <fstream>
#include <iostream>

#include "Editor.h"
#include "logger.h"

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
    markActivity();
    clearSelection();
    ensureCursorVisibleVertically();
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
    markActivity();
    clearSelection();
    ensureCursorVisibleVertically();
}
void Editor::handleReturn()
{
    mBuffer.splitLine(mCursor.row, mCursor.col);
    mCursor.col = 0;
    mCursor.row++;
    markActivity();
    clearSelection();
    ensureCursorVisibleVertically();
}

void Editor::handleLeft(SDL_Keymod mod)
{
    bool shiftHeld = mod & SDL_KMOD_SHIFT;
    if (shiftHeld && !mSelectionActive)
    {
        beginSelection();
    }
    moveCursorLeft();

    if (shiftHeld)
    {
        updateSelection();
    }
    else
    {
        clearSelection();
    }
    ensureCursorVisibleVertically();
    markActivity();
}

void Editor::moveCursorLeft()
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

void Editor::handleRight(SDL_Keymod mod)
{
    bool shiftHeld = mod & SDL_KMOD_SHIFT;
    if (shiftHeld && !mSelectionActive)
    {
        beginSelection();
    }
    moveCursorRight();

    if (shiftHeld)
    {
        updateSelection();
    }
    else
    {
        clearSelection();
    }

    ensureCursorVisibleVertically();
    markActivity();
}

void Editor::moveCursorRight()
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

void Editor::handleUp(SDL_Keymod mod)
{
    bool shiftHeld = mod & SDL_KMOD_SHIFT;
    if (shiftHeld && !mSelectionActive)
    {
        beginSelection();
    }
    moveCursorUp();

    if (shiftHeld)
    {
        updateSelection();
    }
    else
    {
        clearSelection();
    }
    ensureCursorVisibleVertically();
    markActivity();
}

void Editor::moveCursorUp()
{
    if (mCursor.row > 0)
    {
        mCursor.row--;
        bool isUpperShorter = mBuffer.getLineSize(mCursor.row) < mBuffer.getLineSize(mCursor.row + 1);
        bool colGreaterThanLineAbove = mCursor.col >= mBuffer.getLineSize(mCursor.row);
        if (isUpperShorter && colGreaterThanLineAbove)
        {
            mCursor.col = mBuffer.getLineSize(mCursor.row);
        }
    }
}

void Editor::handleDown(SDL_Keymod mod)
{
    bool shiftHeld = mod & SDL_KMOD_SHIFT;
    if (shiftHeld && !mSelectionActive)
    {
        beginSelection();
    }
    moveCursorDown();

    if (shiftHeld)
    {
        updateSelection();
    }
    else
    {
        clearSelection();
    }

    ensureCursorVisibleVertically();
    markActivity();
}

void Editor::moveCursorDown()
{
    if (mCursor.row < mBuffer.getLineCount() - 1)
    {
        mCursor.row++;
        bool isUpperLonger = mBuffer.getLineSize(mCursor.row - 1) > mBuffer.getLineSize(mCursor.row);
        bool colGreaterThanLineBelow = mCursor.col >= mBuffer.getLineSize(mCursor.row);
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

void Editor::ensureCursorVisibleVertically()
{
    if (mCursor.row < mScrollOffsetY)
    {
        mScrollOffsetY = mCursor.row;
    }
    else if (mCursor.row >= mScrollOffsetY + mVisibleRows)
    {
        mScrollOffsetY = mCursor.row - mVisibleRows + 1;
    }

    // LOG_DEBUG() << "offset: " << mScrollOffsetY;
}

void Editor::loadFile(const std::filesystem::path &path)
{
    std::ifstream file{path};

    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open file " << path << "\n";
        return;
    }
    std::vector<std::string> lines;
    std::string currentLine;
    while (std::getline(file, currentLine))
    {
        lines.push_back(currentLine);
    }

    mCurrentFilePath = path;
    mBuffer.setLines(std::move(lines));
    mCursor.row = 0;
    mCursor.col = 0;
    LOG_INFO() << path << " was loaded!";
}

void Editor::saveFileAs(const std::filesystem::path &path)
{
    std::ofstream file{path};

    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open file " << path << "\n";
        return;
    }

    const std::vector<std::string> &lines = mBuffer.getText();

    for (size_t i = 0; i < lines.size(); ++i)
    {
        file << lines[i];

        if (i < lines.size() - 1)
        {
            file << "\n";
        }
    }
    LOG_INFO() << path << " was saved! \n";
}

void Editor::saveFile()
{
    saveFileAs(mCurrentFilePath);
}

void Editor::markActivity()
{
    mActivity = true;
}

bool Editor::consumeActivity()
{
    if (!mActivity)
    {
        return false;
    }

    mActivity = false;
    return true;
}

const Selection &Editor::getSelection() const
{
    return mSelection;
}

void Editor::setSelectionActive(bool b)
{
    mSelectionActive = b;
}

bool Editor::getSelectionActive() const
{
    return mSelectionActive;
}

void Editor::clearSelection()
{
    mSelectionActive = false;
}

void Editor::beginSelection()
{
    mSelection.begin = mCursor;
    mSelection.end = mCursor;
    mSelectionActive = true;
}

void Editor::updateSelection()
{
    mSelection.end = mCursor;
}

Cursor Editor::getCursor() const
{
    return mCursor;
}

const std::string &Editor::getLineString(int i) const
{
    return mBuffer.getLine(i);
}

const uint32_t Editor::getLineCount() const
{
    return mBuffer.getLineCount();
}

const std::vector<std::string> &Editor::getText() const
{
    return mBuffer.getText();
}

void Editor::setVisibleRows(uint32_t rows)
{
    mVisibleRows = rows;
}

const uint32_t &Editor::getVisibleRows() const
{
    return mVisibleRows;
}

const uint32_t &Editor::getScrollOffsetY() const
{
    return mScrollOffsetY;
}

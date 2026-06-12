#include <fstream>
#include <iostream>
#include <optional>

#include <SDL3/SDL_clipboard.h>

#include "Editor.h"
#include "logger.h"

Editor::Editor()
{
    mCursor = Cursor{0, 0};
}

Editor::~Editor()
{
}

void Editor::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_TEXT_INPUT)
    {
        handleTextInput(event.text.text);
    }
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        SDL_Keycode key = event.key.key;
        SDL_Keymod mod = event.key.mod;
        switch (key)
        {
        case SDLK_BACKSPACE:
            handleBackSpace();
            break;
        case SDLK_RETURN:
            handleReturn();
            break;
        case SDLK_LEFT:
            handleLeft(mod);
            break;
        case SDLK_RIGHT:
            handleRight(mod);
            break;
        case SDLK_UP:
            handleUp(mod);
            break;
        case SDLK_DOWN:
            handleDown(mod);
            break;
        case SDLK_TAB:
            handleTab();
            break;
        case SDLK_HOME:
            handleHome(mod);
            break;
        case SDLK_END:
            handleEnd(mod);
            break;
        case SDLK_DELETE:
            handleDelete(mod);
            break;
        case SDLK_COMMA:
            handleComma(mod);
            break;
        case SDLK_A:
            handleA(mod);
            break;
        case SDLK_C:
            handleC(mod);
            break;
        case SDLK_F:
            handleF(mod);
            break;
        case SDLK_S:
            handleS(mod);
            break;
        case SDLK_V:
            handleV(mod);
            break;

        // IO
        case SDLK_F1:
            loadFile("test.txt");
            break;
        case SDLK_F2:
            mTokens = highlighter.tokenize(mBuffer, mLanguage);
            break;
        default:
            break;
        }
    }
}

void Editor::handleTextInput(const std::string &text)
{
    if (isSearchActive())
    {
        mSearch->addToQuery(text);
        updateSearchMatches();
        LOG_DEBUG() << "Search: " << mSearch->getQuery();
    }
    else
    {
        mBuffer.insert(mCursor.row, mCursor.col, text);
        mCursor.col += text.size();
        clearSelection();
        ensureCursorVisibleVertically();
    }
    markActivity();
}

void Editor::handleBackSpace()
{
    if (isSearchActive())
    {
        mSearch->handleBackSpace();
        updateSearchMatches();
    }
    else
    {
        if (mCursor.col > 0)
        {
            mBuffer.erase(mCursor.row, mCursor.col - 1);
            mCursor.col--;
        }
        else if (mCursor.col == 0 && mCursor.row > 0)
        {
            mCursor.row--;
            moveCursorToEndCol();
            mBuffer.mergeWithNext(mCursor.row);
            clearSelection();
            ensureCursorVisibleVertically();
        }
    }
    markActivity();
}
void Editor::handleReturn()
{
    if (!isSearchActive())
    {
        mBuffer.splitLine(mCursor.row, mCursor.col);
        moveCursorToBeginCol();
        mCursor.row++;
        markActivity();
        clearSelection();
        ensureCursorVisibleVertically();
    }
}

void Editor::handleDelete(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mSearch->handleDelete();
        updateSearchMatches();
    }
    else
    {
        bool shiftHeld = mod & SDL_KMOD_SHIFT;

        // shift + del deletes entire line
        if (shiftHeld)
        {
            mBuffer.eraseRange(mCursor.row, 0, mBuffer.getLineSize(mCursor.row));
            mBuffer.mergeWithNext(mCursor.row);
            moveCursorToBeginCol();
        }
        else
        {
            if (mCursor.col < mBuffer.getLineSize(mCursor.row))
            {
                mBuffer.erase(mCursor.row, mCursor.col);
            }
            else if (mCursor.col == mBuffer.getLineSize(mCursor.row) && mCursor.row < mBuffer.getLineCount() - 1)
            {
                mBuffer.mergeWithNext(mCursor.row);
            }
        }

        clearSelection();
        ensureCursorVisibleVertically();
    }
    markActivity();
}

void Editor::handleLeft(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mSearch->handleLeft();
    }
    else
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
    }
    markActivity();
}

void Editor::handleRight(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mSearch->handleRight();
    }
    else
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
    }
    markActivity();
}

void Editor::handleUp(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mCursor = mSearch->handleUp(mCursor);
    }
    else
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
    }
    ensureCursorVisibleVertically();
    markActivity();
}

void Editor::handleDown(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mCursor = mSearch->handleDown(mCursor);
    }
    else
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
    }

    ensureCursorVisibleVertically();
    markActivity();
}

void Editor::handleHome(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mSearch->resetCursor();
    }
    else
    {
        bool shiftHeld = mod & SDL_KMOD_SHIFT;
        bool ctrlHeld = mod & SDL_KMOD_CTRL;

        if (shiftHeld && !mSelectionActive)
        {
            beginSelection();
        }
        if (ctrlHeld)
        {
            moveCursorToFirstRow();
        }

        moveCursorToBeginCol();

        if (shiftHeld)
        {
            updateSelection();
        }
        else
        {
            clearSelection();
        }

        ensureCursorVisibleVertically();
    }

    markActivity();
}

void Editor::handleEnd(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mSearch->handleEnd();
    }
    else
    {
        bool shiftHeld = mod & SDL_KMOD_SHIFT;
        bool ctrlHeld = mod & SDL_KMOD_CTRL;

        if (shiftHeld && !mSelectionActive)
        {
            beginSelection();
        }
        if (ctrlHeld)
        {
            moveCursorToLastRow();
        }

        moveCursorToEndCol();

        if (shiftHeld)
        {
            updateSelection();
        }
        else
        {
            clearSelection();
        }

        ensureCursorVisibleVertically();
    }

    markActivity();
}
/*
    Using Comma here, because on German keyboard it is interpreted as a comma always, even when pressing shift it is just shift+comma, so now ctrl + comma inserts ; at the end of the line
*/
void Editor::handleComma(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld)
    {
        mBuffer.insert(mCursor.row, mBuffer.getLine(mCursor.row).size(), ";");
        moveCursorToEndCol();
    }
}

void Editor::handleA(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld && !mSelectionActive)
    {
        mSelectionActive = true;
        mSelection.begin = {0, 0};
        mSelection.end = {mBuffer.getLineCount() - 1, mBuffer.getLine(mBuffer.getLineCount() - 1).size()};
        moveCursorToLastRow();
        moveCursorToEndCol();
    }
}

void Editor::handleC(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        const std::string &text = getSelectedText();
        LOG_DEBUG() << text;
        SDL_SetClipboardText(text.c_str());
    }
}

void Editor::handleF(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        if (!isSearchActive())
        {
            mSearch.emplace();
            LOG_DEBUG() << "Search activated!";
        }
        else
        {
            mSearch.reset();
            mSearch->resetCursor();
            LOG_DEBUG() << "Search deactivated!";
        }
    }
}

void Editor::handleS(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        saveFile();
    }
}

void Editor::handleV(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        const std::string &text = SDL_GetClipboardText();
        LOG_DEBUG() << text;
        mCursor = mBuffer.insertFormatted(mCursor.row, mCursor.col, text);
    }
}

void Editor::handleTab()
{
    handleTextInput("\t");
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
        moveCursorToEndCol();
    }
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
        moveCursorToBeginCol();
    }
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
            moveCursorToEndCol();
        }
    }
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
            moveCursorToEndCol();
        }
    }
}

void Editor::moveCursorToBeginCol()
{
    mCursor.col = 0;
}

void Editor::moveCursorToEndCol()
{
    mCursor.col = mBuffer.getLineSize(mCursor.row);
}

void Editor::moveCursorToFirstRow()
{
    mCursor.row = 0;
}

void Editor::moveCursorToLastRow()
{
    mCursor.row = mBuffer.getLineCount() - 1;
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
    LOG_INFO() << path << " was saved!";
}

void Editor::saveFile()
{
    saveFileAs(mCurrentFilePath);
}

bool Editor::isSearchActive() const
{
    return mSearch.has_value();
}

const SearchSession &Editor::getSearch() const
{
    return *mSearch;
}

void Editor::updateSearchMatches()
{
    if (!mSearch)
        return;
    mSearch->setMatches(mSearchEngine.find(mBuffer, mSearch->getQuery()));

    if (mSearch->getMatches().size() > 0)
    {
        SearchMatch match = mSearch->getMatches()[0];
        mCursor.row = (size_t)match.row;
        mCursor.col = (size_t)match.col;
    }
}

std::vector<std::vector<Token>> Editor::getTokens() const
{
    return mTokens;
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

const std::string Editor::getSelectedText() const
{
    Selection selection = mSelection.normalized();
    std::string test = "";
    if (mSelectionActive)
    {

        LOG_DEBUG() << selection.begin << " -> " << selection.end;
        test = mBuffer.getTextSlice(selection.begin, selection.end);
        LOG_DEBUG() << test;
    }
    return test;
}

Cursor Editor::getCursor() const
{
    return mCursor;
}

const std::string &Editor::getLineString(int i) const
{
    return mBuffer.getLine(i);
}

uint32_t Editor::getLineCount() const
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
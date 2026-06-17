#include <fstream>
#include <iostream>
#include <optional>

#include <SDL3/SDL_clipboard.h>

#include "Editor.h"
#include "logger.h"
#include "util.h"

Editor::Editor()
{
    mCursor = Cursor{0, 0};
    mTerminal = std::make_unique<Terminal>();
}

Editor::~Editor()
{
}

void Editor::handlePaneKeyHandler(const SDL_Event &event)
{
    if (mFocus == Focus::Editor)
    {
        handleKey(event);
    }
    else if (mFocus == Focus::Terminal)
    {
        mTerminal->handleKey(event);
    }
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
            handleBackSpace(mod);
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
        case SDLK_X:
            handleX(mod);
            break;
        case SDLK_Y:
            handleY(mod);
            break;
        case SDLK_Z:
            handleZ(mod);
            break;

        // IO
        case SDLK_F1:
            loadFile("test.txt");
            break;
        case SDLK_F2:
            mTokens = mHighlighter.tokenize(mBuffer, mLanguage);
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
        insertText(mCursor, text);
        mCursor.col += text.size();
        clearSelection();
        ensureCursorVisibleVertically();
        updateTokens();
    }
    markActivity();
}

void Editor::handleBackSpace(SDL_Keymod mod)
{
    if (isSearchActive())
    {
        mSearch->handleBackSpace();
        updateSearchMatches();
    }
    else
    {
        if (mSelectionActive)
        {
            // mBuffer.eraseRangeMultiRow(mSelection.normalized());
            Selection normSel = mSelection.normalized();
            Position targetPos = normSel.begin;
            std::string textToDelete = mBuffer.getTextSlice(normSel.begin, normSel.end);
            deleteText(targetPos, textToDelete);
            mCursor = targetPos;
        }
        else
        {
            bool ctrlHeld = mod & SDL_KMOD_CTRL;
            if (ctrlHeld)
            {
                // delete word left to cursor
                Range leftWord = findWordLeftOfIndex(mBuffer.getLine(mCursor.row).substr(0, mCursor.col));
                deleteText({mCursor.row, leftWord.start}, mBuffer.getLine(mCursor.row).substr(leftWord.start, leftWord.end - leftWord.start));
                // mBuffer.eraseRange(mCursor.row, leftWord);
                mCursor.col = leftWord.start;
            }
            else
            {
                if (mCursor.row == 0 && mCursor.col == 0)
                    return;
                Position targetPos;
                std::string textToDelete;
                if (mCursor.col > 0)
                {
                    targetPos = {mCursor.row, mCursor.col - 1};
                    textToDelete = mBuffer.getLine(mCursor.row).substr(mCursor.col - 1, 1);
                }
                else if (mCursor.col == 0 && mCursor.row > 0)
                {
                    targetPos = {mCursor.row - 1, mBuffer.getLineSize(mCursor.row - 1)};
                    textToDelete = "\n";
                }
                deleteText(targetPos, textToDelete);
                mCursor = targetPos;
            }
        }
        clearSelection();
        ensureCursorVisibleVertically();
        updateTokens();
    }
    markActivity();
}
void Editor::handleReturn()
{
    if (!isSearchActive())
    {
        insertText(mCursor, "\n");
        moveCursorToBeginCol();
        mCursor.row++;
        markActivity();
        clearSelection();
        ensureCursorVisibleVertically();
        updateTokens();
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
        bool ctrlHeld = mod & SDL_KMOD_CTRL;

        // shift + del deletes entire line
        if (shiftHeld)
        {
            Position targetPos = {mCursor.row, 0};
            std::string textToDelete = mBuffer.getLine(mCursor.row);
            if (mCursor.row < mBuffer.getText().size() - 1)
            {
                textToDelete += "\n";
            }
            deleteText(targetPos, textToDelete);
            mCursor = targetPos;
            // mBuffer.eraseRange(mCursor.row, 0, mBuffer.getLineSize(mCursor.row));
            // mBuffer.mergeWithNext(mCursor.row);
            // moveCursorToBeginCol();
        }
        else if (ctrlHeld)
        {
            // delete word right to cursor
            Range rightWord = findWordRightOfIndex(mBuffer.getLine(mCursor.row).substr(mCursor.col, mBuffer.getLineSize(mCursor.row)));
            deleteText({mCursor.row, mCursor.col + rightWord.start}, mBuffer.getLine(mCursor.row).substr(mCursor.col + rightWord.start, rightWord.end - rightWord.start));
            // mBuffer.eraseRange(mCursor.row, mCursor.col + rightWord.start, mCursor.col + rightWord.end);
        }
        else
        {
            Position targetPos;
            std::string textToDelete;
            if (mCursor.col < mBuffer.getLineSize(mCursor.row))
            {
                targetPos = mCursor;
                textToDelete = mBuffer.getLine(mCursor.row).substr(mCursor.col, 1);
                // mBuffer.erase(mCursor.row, mCursor.col);
            }
            else if (mCursor.col == mBuffer.getLineSize(mCursor.row) && mCursor.row < mBuffer.getLineCount() - 1)
            {
                targetPos = {mCursor.row, mBuffer.getLineSize(mCursor.row)};
                textToDelete = "\n";
                // mBuffer.mergeWithNext(mCursor.row);
            }
            deleteText(targetPos, textToDelete);
        }

        clearSelection();
        ensureCursorVisibleVertically();
        updateTokens();
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
        moveCursorLeft(mod);

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
        moveCursorRight(mod);

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
        insertText({mCursor.row, mBuffer.getLine(mCursor.row).size()}, ";");
        moveCursorToEndCol();
        updateTokens();
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

void Editor::handleT(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        mTerminalVisible = !mTerminalVisible;
        mFocus = mTerminalVisible ? Focus::Terminal : Focus::Editor;
    }
}

void Editor::handleV(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        const std::string &text = SDL_GetClipboardText();
        LOG_DEBUG() << text;
        // not using insertText here because of convenience of getting the new cursor
        auto action = std::make_unique<InsertAction>(mCursor, text);
        mCursor = mBuffer.insertFormatted(mCursor.row, mCursor.col, text);
        mUndoManager.push(std::move(action));

        updateTokens();
    }
}

void Editor::handleX(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        const std::string &text = getSelectedText();
        SDL_SetClipboardText(text.c_str());
        Selection normSel = mSelection.normalized();
        Position targetPos = normSel.begin;
        deleteText(targetPos, text);
        mCursor = targetPos;
        // mBuffer.eraseRangeMultiRow(mSelection.normalized());
        // mCursor = mSelection.normalized().begin;
        clearSelection();
        ensureCursorVisibleVertically();
        updateTokens();
    }
}

void Editor::handleY(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld && mUndoManager.canRedo())
    {
        mCursor = mUndoManager.redo(mBuffer);
        updateTokens();
        ensureCursorVisibleVertically();
        markActivity();
    }
}

void Editor::handleZ(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld && mUndoManager.canUndo())
    {
        mCursor = mUndoManager.undo(mBuffer);
        updateTokens();
        ensureCursorVisibleVertically();
        markActivity();
    }
}

/**
 * @brief Safe wrapper to inject text into the buffer AND record it in history.
 * * ALWAYS use this function in your UI key handlers instead of calling the buffer directly.
 * It modifies the file and saves the action to the Undo stack in one unified step.
 */
void Editor::insertText(Position pos, const std::string &text)
{
    if (text.empty())
        return;

    auto action = std::make_unique<InsertAction>(pos, text);
    action->redo(mBuffer);
    mUndoManager.push(std::move(action));
}

/**
 * @brief Safe wrapper to erase text from the buffer AND record it in history.
 * * ALWAYS use this function for backspace or delete keys. It creates a DeleteAction,
 * runs the erasure, and logs it so Ctrl+Z can restore the deleted text later.
 */
void Editor::deleteText(Position pos, const std::string &text)
{
    if (text.empty()) return;

        auto action = std::make_unique<DeleteAction>(pos, text);
        action->redo(mBuffer); 
        
        mUndoManager.push(std::move(action));
}

void Editor::handleTab()
{
    handleTextInput("\t");
}

void Editor::moveCursorLeft(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld)
    {
        Range wordLeft = findWordLeftOfIndex(mBuffer.getLine(mCursor.row).substr(0, mCursor.col));
        mCursor.col = wordLeft.start;
    }
    else
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
}

void Editor::moveCursorRight(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    if (ctrlHeld)
    {
        Range wordRight = findWordRightOfIndex(mBuffer.getLine(mCursor.row).substr(mCursor.col, mBuffer.getLineSize(mCursor.row)));
        mCursor.col = mCursor.col + wordRight.end;
    }
    else
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
}
// should work because new visible rows isnt updated yet
void Editor::adjustCursor(uint32_t rows)
{
    if (isTerminalVisible() && mCursor.row > rows)
    {
        mCursor.row -= mVisibleRows - rows;
    }
}

void Editor::loadFile(const std::filesystem::path &path)
{
    std::ifstream file{path};

    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open file " << path << "\n";
        return;
    }
    if (path.extension() == ".cpp" || path.extension() == ".h")
    {
        mLanguage = Language::Cpp;
    }
    else
    {
        mLanguage = Language::PlainText;
    }

    std::vector<std::string> lines;
    std::string currentLine;
    while (std::getline(file, currentLine))
    {
        lines.push_back(currentLine);
    }

    mCurrentFilePath = path;
    mBuffer.setLines(std::move(lines));
    updateTokens();
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

void Editor::updateTokens()
{
    mTokens.clear();
    mTokens = mHighlighter.tokenize(mBuffer, mLanguage);
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

bool Editor::isTerminalVisible() const
{
    return mTerminalVisible;
}

void Editor::switchFocus()
{
    if (mFocus == Focus::Editor)
    {
        mFocus = Focus::Terminal;
    }
    else
    {
        mFocus = Focus::Editor;
    }
}

const Terminal &Editor::getTerminalConst() const
{
    return *mTerminal;
}

Terminal &Editor::getTerminal()
{
    return *mTerminal;
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

void Editor::update()
{
    if (std::optional<CommandRequest> request = mTerminal->consumeRequest())
    {
        handleRequest(*request);
    }
}

void Editor::handleRequest(const CommandRequest &request)
{
    switch (request.type)
    {
    case CommandRequestType::Quit:
        mPendingRequest = {CommandRequestType::Quit, ""};
        break;
    case CommandRequestType::SaveFile:
        saveFile();
        break;
    case CommandRequestType::ChangeLanguage:
        if (request.request == "cpp")
        {
            mLanguage = Language::Cpp;
        }
        updateTokens();
        break;

    case CommandRequestType::OpenFile:
        loadFile(request.request);
        break;
    case CommandRequestType::Error:
        LOG_ERROR() << "Error: " << request.request;
        break;

    default:
        break;
    }
}

std::optional<CommandRequest> Editor::consumeRequest()
{
    auto res = mPendingRequest;
    mPendingRequest.reset();
    return res;
}

#include <sstream>

#include "Terminal.h"
#include "logger.h"
#include "util.h"

void Terminal::handleKey(const SDL_Event &event)
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
        case SDLK_RETURN:
            handleReturn();
            break;
        case SDLK_BACKSPACE:
            handleBackSpace();
            break;
        case SDLK_UP:
            handleUp(mod);
            break;
        case SDLK_DOWN:
            handleDown(mod);
            break;
        case SDLK_DELETE:
            handleDelete();
            break;
        case SDLK_LEFT:
            handleLeft();
            break;
        case SDLK_RIGHT:
            handleRight();
            break;
        case SDLK_HOME:
            handleHome();
            break;
        case SDLK_END:
            handleEnd();
            break;
        }
    }
}

void Terminal::handleTextInput(const std::string &text)
{
    mInput.insert(0, mCursor, text);
    mCursor += text.size();
}

void Terminal::handleBackSpace()
{
    if (mCursor > 0)
    {
        mInput.erase(0, mCursor - 1);
        mCursor--;
    }
}

void Terminal::handleDelete()
{
    if (mCursor < mInput.getLineSize(0))
    {
        mInput.erase(0, mCursor);
    }
}

void Terminal::handleReturn()
{
    if (mInput.getText().size() == 0 || mInput.getLine(0).size() == 0)
        return;
    std::string trimmedInput = trim(mInput.getLine(0));

    std::string command;
    std::vector<std::string> options;
    if (!trimmedInput.empty() && !trimmedInput.starts_with("!"))
    {
        std::stringstream ss(trimmedInput);
        ss >> command;
        std::string option;
        while (ss >> option)
        {
            options.push_back(option);
        }
        mProcessor.executeCommand(command, options);
    }
    else if (trimmedInput.starts_with("!"))
    {
        mProcessor.executeCommand(trimmedInput, {});
    }
    mCmdHistory.push_back(trimmedInput);
    mInput.clear();
    mCursor = 0;
    mHistoryIndex = 0;
}

void Terminal::handleUp(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        if (mScrollOffset < mProcessor.getOutput().getText().size() - 1 - mVisibleRows)
        {
            mScrollOffset++;
        }
    }
    else
    {
        if (mCmdHistory.size() > 0 && mHistoryIndex < mCmdHistory.size())
        {
            if (mHistoryIndex == 0 && mInput.getLineSize(0) > 0)
                mSaveInput = mInput.getLine(0);
            mHistoryIndex++;
            mInput.clear();
            mCursor = 0;
            uint32_t targetIndex = mCmdHistory.size() - mHistoryIndex;
            handleTextInput(mCmdHistory[targetIndex]);
        }
    }
}

void Terminal::handleDown(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        if (mScrollOffset > 0)
        {
            mScrollOffset--;
        }
    }
    else
    {
        if (mCmdHistory.size() > 0 && mHistoryIndex > 0)
        {
            mInput.clear();
            mCursor = 0;
            mHistoryIndex--;
            if (mHistoryIndex == 0)
            {
                handleTextInput(mSaveInput);
            }
            else
            {
                uint32_t targetIndex = mCmdHistory.size() - mHistoryIndex;
                handleTextInput(mCmdHistory[targetIndex]);
            }
        }
    }
}

void Terminal::handleRight()
{
    if (mCursor < mInput.getLineSize(0))
    {
        mCursor++;
    }
}

void Terminal::handleLeft()
{
    if (mCursor > 0)
    {
        mCursor--;
    }
}

void Terminal::handleHome()
{
    mCursor = 0;
}

void Terminal::handleEnd()
{
    mCursor = mInput.getLineSize(0);
}

std::string Terminal::getInput() const
{
    if (mInput.getLineSize(0) > 0)
    {
        return mInput.getLine(0);
    }
    return std::string();
}

TextBuffer Terminal::getOutput() const
{
    return mProcessor.getOutput();
}

uint32_t Terminal::getCursor() const
{
    return mCursor;
}

uint32_t Terminal::getScrollOffset() const
{
    return mScrollOffset;
}

uint32_t Terminal::getVisibleRows() const
{
    return mVisibleRows;
}

void Terminal::setVisibleRows(uint32_t rows)
{
    mVisibleRows = rows;
}

std::optional<CommandRequest> Terminal::consumeRequest()
{
    return mProcessor.consumeRequest();
}

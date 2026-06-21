#include <sstream>

#include "Terminal.h"
#include "util.h"

void Terminal::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        SDL_Keycode key = event.key.key;
        SDL_Keymod mod = event.key.mod;
        switch (key)
        {
        case SDLK_RETURN:
            handleReturn();
            return;
        case SDLK_UP:
            handleUp(mod);
            return;
        case SDLK_DOWN:
            handleDown(mod);
            return;
        default:
            break;
        }
    }
    mInput.handleKey(event);
}

void Terminal::handleReturn()
{
    if (mInput.getText().empty())
        return;
    std::string trimmedInput = trim(mInput.getText());

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
            if (mHistoryIndex == 0 && !mInput.getText().empty())
                mSaveInput = mInput.getText();
            mHistoryIndex++;
            mInput.clear();
            uint32_t targetIndex = mCmdHistory.size() - mHistoryIndex;
            mInput.insert(mCmdHistory[targetIndex]);
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
            mHistoryIndex--;
            if (mHistoryIndex == 0)
            {
                mInput.insert(mSaveInput);
            }
            else
            {
                uint32_t targetIndex = mCmdHistory.size() - mHistoryIndex;
                mInput.insert(mCmdHistory[targetIndex]);
            }
        }
    }
}

std::string Terminal::getInput() const
{
    return mInput.getText();
}

TextBuffer Terminal::getOutput() const
{
    return mProcessor.getOutput();
}

uint32_t Terminal::getCursor() const
{
    return mInput.getCursor();
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

bool Terminal::hasSelection() const
{
    return mInput.hasSelection();
}

TextSelection Terminal::getSelection() const
{
    return mInput.getSelection();
}

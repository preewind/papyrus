#include "Terminal.h"
#include "logger.h"


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
    LOG_DEBUG() << "Text: " << mInput.getLine(0);
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
    const auto &result = mProcessor.executeCommand(mInput.getLine(0), {}).output;
    for (auto &line : result.getText())
    {
        LOG_DEBUG() << line;
    }
    mInput.clear();
    mCursor = 0;
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

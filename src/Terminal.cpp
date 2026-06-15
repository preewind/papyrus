#include "Terminal.h"
#include "logger.h"

Terminal::Terminal()
{
    mCommands["build"] = [this](const auto &args)
    {
        return buildCommand(args);
    };
    mCommands["flex"] = [this](const auto &args)
    {
        return flexCommand(args);
    };
}
Terminal::Terminal(uint32_t rows)
{
    mCommands["build"] = [this](const auto &args)
    {
        return buildCommand(args);
    };
    mCommands["flex"] = [this](const auto &args)
    {
        return flexCommand(args);
    };
    mVisibleRows = rows;
};

CommandResult Terminal::executeCommand(const std::string &name, const std::vector<std::string> &args)
{
    if(name.starts_with("!")){
        return executeShell(name.substr(1));
    }

    auto it = mCommands.find(name);
    if (it == mCommands.end())
    {
        return {
            false,
            {"Unknown command"}};
    }
    return it->second(args);
}

CommandResult Terminal::executeShell(const std::string &commandLine)
{
    std::thread([this, commandLine]()
                {
                    FILE *pipe = popen(commandLine.c_str(), "r");
                    if (!pipe)
                    {
                        return;
                    }
                    char buffer[256];

                    while (fgets(buffer, sizeof(buffer), pipe))
                    {
                        std::string line(buffer);
                        LOG_DEBUG() << "line: " << line;
                        while (!line.empty() && (line.back() == '\n' || line.back() == '\r'))
                        {
                            line.pop_back();
                        }
                        {
                            std::lock_guard<std::mutex> lock(mOutputMutex);
                            mOutput.addLine(line);
                        }
                    }
                    pclose(pipe); })
        .detach();

    return {true, mOutput};
}

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
    mOutput = executeCommand(mInput.getLine(0), {}).output;
    for (auto &line : mOutput.getText())
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
        if (mScrollOffset < mOutput.getText().size() - 1 - mVisibleRows)
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
    std::lock_guard<std::mutex> lock(mOutputMutex);
    return mOutput;
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

CommandResult Terminal::buildCommand(const std::vector<std::string> &args)
{
    (void)args;
    return executeShell("./run.sh -r");
}

CommandResult Terminal::flexCommand(const std::vector<std::string> &args)
{
    (void)args;
    return executeShell("wc -l src/*cpp");
}

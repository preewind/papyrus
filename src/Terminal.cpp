#include "Terminal.h"
#include "logger.h"

Terminal::Terminal() {
    mCommands["build"] = [this](const auto &args)
    {
        return buildCommand(args);
    };
};

CommandResult Terminal::executeCommand(const std::string& name, const std::vector<std::string>& args)
{
    LOG_DEBUG() << "Hello from Terminal!";
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
    FILE *pipe = popen(commandLine.c_str(), "r");
    if (!pipe)
    {
        return {false, {"Failed to execute command"}};
    }
    std::vector<std::string> output;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        output.push_back(buffer);
    }
    pclose(pipe);
    return {true, output};
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
        switch (key)
        {
        case SDLK_RETURN:
            handleReturn();
            break;
        case SDLK_BACKSPACE:
            handleBackSpace();
            break;
        case SDLK_UP:
            handleUp();
            break;
        case SDLK_DOWN:
            handleDown();
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

void Terminal::handleReturn()
{
    auto result = executeCommand(mInput.getLine(0), {});
    for(auto &line: result.output){
        LOG_DEBUG() << line;
    }
    mInput.clear();
    mCursor = 0;
}

void Terminal::handleUp()
{
}

void Terminal::handleDown()
{
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
    return mOutput;
}

uint32_t Terminal::getCursor() const
{
    return mCursor;
}

CommandResult Terminal::buildCommand(const std::vector<std::string> &args)
{
    (void) args;
    return executeShell("./run.sh -r -run");
}

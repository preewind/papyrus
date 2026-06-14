#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <SDL3/SDL_events.h>

#include "TextBuffer.h"
#include "types.h"
#include <functional>

struct CommandResult
{
    bool success;
    std::vector<std::string> output;
};

class Terminal{

public:
    Terminal();
    CommandResult executeCommand(const std::string& name, const std::vector<std::string>& args);
    CommandResult executeShell(const std::string& commandLine);
    void handleKey(const SDL_Event &event);
    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleDelete();
    void handleReturn();
    void handleUp();
    void handleDown();
    void handleRight();
    void handleLeft();
    void handleHome();
    void handleEnd();
    std::string getInput() const;
    TextBuffer getOutput() const;
    uint32_t getCursor() const;


    CommandResult buildCommand(const std::vector<std::string>& args);


private:
    TextBuffer mInput;
    TextBuffer mOutput;
    uint32_t mCursor = 0;
    std::unordered_map<std::string, std::function<CommandResult(const std::vector<std::string>&)>> mCommands;
};
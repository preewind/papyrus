#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>

#include <SDL3/SDL_events.h>

#include "TextBuffer.h"
#include "types.h"
#include <functional>

struct CommandResult
{
    bool success;
    TextBuffer output;
};

class Terminal{

public:
    Terminal();
    Terminal(uint32_t rows);
    CommandResult executeCommand(const std::string& name, const std::vector<std::string>& args);
    CommandResult executeShell(const std::string& commandLine);
    void handleKey(const SDL_Event &event);
    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleDelete();
    void handleReturn();
    void handleUp(SDL_Keymod mod);
    void handleDown(SDL_Keymod mod);
    void handleRight();
    void handleLeft();
    void handleHome();
    void handleEnd();
    std::string getInput() const;
    TextBuffer getOutput() const;
    uint32_t getCursor() const;
    uint32_t getScrollOffset() const;
    uint32_t getVisibleRows() const;


    CommandResult buildCommand(const std::vector<std::string>& args);
    CommandResult flexCommand(const std::vector<std::string>& args);


private:
    TextBuffer mInput;
    TextBuffer mOutput;
    mutable std::mutex mOutputMutex;
    uint32_t mCursor = 0;
    uint32_t mScrollOffset = 0;
    uint32_t mVisibleRows;
    std::unordered_map<std::string, std::function<CommandResult(const std::vector<std::string>&)>> mCommands;
};
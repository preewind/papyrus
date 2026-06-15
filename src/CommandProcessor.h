#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <unordered_map>
#include <optional>

#include "types.h"
#include "TextBuffer.h"

struct CommandResult
{
    bool success;
    TextBuffer output;
};

class CommandProcessor
{
public:
    CommandProcessor();
    CommandResult executeCommand(const std::string &name, const std::vector<std::string> &args);
    CommandResult executeShell(const std::string &commandLine);
    CommandResult buildCommand(const std::vector<std::string> &args);
    CommandResult flexCommand(const std::vector<std::string> &args);
    CommandResult quitCommand(const std::vector<std::string> &args);
    CommandResult openFileCommand(const std::vector<std::string> &args);
    CommandResult saveCommand(const std::vector<std::string> &args);
    CommandResult changeLanguageCommand(const std::vector<std::string> &args);
    std::optional<CommandRequest> consumeRequest();

    TextBuffer getOutput() const;
private:
    TextBuffer mOutput;
    mutable std::mutex mOutputMutex;
    std::unordered_map<std::string, std::function<CommandResult(const std::vector<std::string> &)>> mCommands;
    std::optional<CommandRequest> mPendingRequest;
};
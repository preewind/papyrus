#pragma once

#include <mutex>
#include <string>
#include <vector>

#include "types.h"
#include "TextBuffer.h"
#include "CommandRegistry.h"

class CommandProcessor
{
public:
    void registerCommand(CommandDefinition def);
    CommandResult executeCommand(const std::string &name, const std::vector<std::string> &args);

    const TextBuffer &getOutput() const;

private:
    CommandResult executeShell(const std::string &commandLine);
    void addOutputLine(const std::string &line);

    TextBuffer mOutput;
    mutable std::mutex mOutputMutex;
    CommandRegistry mRegistry;
};
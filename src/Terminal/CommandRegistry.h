#pragma once

#include <functional>
#include <string>
#include <vector>

#include "types.h"

struct CommandDefinition
{
    std::string name;
    std::string description;
    std::string usage;

    // Exactly one of these should be set:
    std::function<CommandResult(const std::vector<std::string> &)> handler{};
    std::string shellScript{}; // If set, runs as a shell command (ignores handler)
};

class CommandRegistry
{
public:
    void registerCommand(CommandDefinition def);
    const CommandDefinition *find(const std::string &name) const;
    std::string buildHelpText() const;
    const std::vector<CommandDefinition> &commands() const;

private:
    std::vector<CommandDefinition> mCommands;
};

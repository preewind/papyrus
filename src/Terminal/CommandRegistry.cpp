#include "CommandRegistry.h"

#include <algorithm>
#include <sstream>

void CommandRegistry::registerCommand(CommandDefinition def)
{
    // Replace existing command with same name if present
    for (auto &existing : mCommands)
    {
        if (existing.name == def.name)
        {
            existing = std::move(def);
            return;
        }
    }
    mCommands.push_back(std::move(def));
}

const CommandDefinition *CommandRegistry::find(const std::string &name) const
{
    for (const auto &cmd : mCommands)
    {
        if (cmd.name == name)
        {
            return &cmd;
        }
    }
    return nullptr;
}

std::string CommandRegistry::buildHelpText() const
{
    // Calculate column widths for alignment
    size_t nameColWidth = 0;
    for (const auto &cmd : mCommands)
    {
        size_t w = cmd.name.size() + (cmd.usage.empty() ? 0 : 1 + cmd.usage.size());
        nameColWidth = std::max(nameColWidth, w);
    }
    // Include the !<cmd> entry
    nameColWidth = std::max(nameColWidth, std::string{"!<cmd>"}.size());
    const size_t padding = 3;

    std::ostringstream out;
    out << "Available commands:";
    for (const auto &cmd : mCommands)
    {
        out << "\n  " << cmd.name;
        if (!cmd.usage.empty())
        {
            out << " " << cmd.usage;
        }
        size_t used = cmd.name.size() + (cmd.usage.empty() ? 0 : 1 + cmd.usage.size());
        out << std::string(nameColWidth - used + padding, ' ') << cmd.description;
    }
    out << "\n  !<cmd>" << std::string(nameColWidth - 6 + padding, ' ') << "Execute a shell command";
    out << "\n  help" << std::string(nameColWidth - 4 + padding, ' ') << "Show this help message";
    return out.str();
}

const std::vector<CommandDefinition> &CommandRegistry::commands() const
{
    return mCommands;
}

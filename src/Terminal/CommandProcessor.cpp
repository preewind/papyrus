#include <sstream>
#include <thread>

#include "CommandProcessor.h"

void CommandProcessor::registerCommand(CommandDefinition def)
{
    mRegistry.registerCommand(std::move(def));
}

CommandResult CommandProcessor::executeCommand(const std::string &name, const std::vector<std::string> &args)
{
    // Shell passthrough: !command
    if (name.starts_with("!"))
    {
        auto result = executeShell(name.substr(1));
        addOutputLine(result.message);
        return result;
    }

    // Built-in help: always available, auto-generated from registry
    if (name == "help")
    {
        const std::string helpText = mRegistry.buildHelpText();
        std::istringstream ss(helpText);
        std::string line;
        while (std::getline(ss, line))
        {
            addOutputLine(line);
        }
        return {true, ""};
    }

    const CommandDefinition *def = mRegistry.find(name);
    if (!def)
    {
        const std::string msg = "Unknown command: " + name + " (type 'help' for a list)";
        addOutputLine(msg);
        return {false, msg};
    }

    CommandResult result;
    if (!def->shellScript.empty())
    {
        result = executeShell(def->shellScript);
    }
    else if (def->handler)
    {
        result = def->handler(args);
    }
    else
    {
        result = {false, "Command '" + name + "' has no handler"};
    }

    if (!result.message.empty())
    {
        addOutputLine(result.message);
    }
    return result;
}

CommandResult CommandProcessor::executeShell(const std::string &commandLine)
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

    return {true, "Executed: " + commandLine};
}

void CommandProcessor::addOutputLine(const std::string &line)
{
    mOutput.addLine(line);
}

const TextBuffer &CommandProcessor::getOutput() const
{
    std::lock_guard<std::mutex> lock(mOutputMutex);
    return mOutput;
}

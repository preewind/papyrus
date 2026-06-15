#include "CommandProcessor.h"
#include "logger.h"

CommandProcessor::CommandProcessor()
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

CommandResult CommandProcessor::executeCommand(const std::string &name, const std::vector<std::string> &args)
{
    if(name.starts_with("!")){
        return executeShell(name.substr(1));
    }

    auto it = mCommands.find(name);
    if (it == mCommands.end())
    {
        mOutput.addLine("Unknown command");
        return {
            false,
            {"Unknown command"}};
    }
    return it->second(args);
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

CommandResult CommandProcessor::buildCommand(const std::vector<std::string> &args)
{
    (void)args;
    return executeShell("./run.sh -r");
}

CommandResult CommandProcessor::flexCommand(const std::vector<std::string> &args)
{
    (void)args;
    return executeShell("wc -l src/*cpp");
}

TextBuffer CommandProcessor::getOutput() const
{
    std::lock_guard<std::mutex> lock(mOutputMutex);
    return mOutput;
}

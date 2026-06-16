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
    mCommands["quit"] = [this](const auto &args)
    {
        return quitCommand(args);
    };
    mCommands["open"] = [this](const auto &args)
    {
        return openFileCommand(args);
    };
    mCommands["save"] = [this](const auto &args)
    {
        return saveCommand(args);
    };
    mCommands["cL"] = [this](const auto &args)
    {
        return changeLanguageCommand(args);
    };
}

CommandResult CommandProcessor::executeCommand(const std::string &name, const std::vector<std::string> &args)
{
    if (name.starts_with("!"))
    {
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
    CommandResult result = it->second(args);
    mOutput.addLine(result.message);
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

    return {true, std::string("Executed Command: " + commandLine)};
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

CommandResult CommandProcessor::quitCommand(const std::vector<std::string> &args)
{
    (void)args;
    mPendingRequest = {CommandRequestType::Quit, ""};
    return {true, {"Quit!"}};
}

CommandResult CommandProcessor::openFileCommand(const std::vector<std::string> &args)
{
    mPendingRequest = {CommandRequestType::OpenFile, args[0]};
    return {true, {"Opened file"}};
}

CommandResult CommandProcessor::saveCommand(const std::vector<std::string> &args)
{
    (void)args;
    mPendingRequest = {CommandRequestType::SaveFile, ""};
    return {true, {"Saved current file!"}};
}

CommandResult CommandProcessor::changeLanguageCommand(const std::vector<std::string> &args)
{
    CommandRequest result;
    if(!args.empty()){
        mPendingRequest = {CommandRequestType::ChangeLanguage, args[0]};
        return {true, {std::string("Changed language to " + args[0])}};   
    }
    else{
        mPendingRequest = {CommandRequestType::Error, "No language provided!"};
        return {false, {"No language provided!"}};
    }
    
    return {false, {"Something went wrong while changing language!"}};
}

std::optional<CommandRequest> CommandProcessor::consumeRequest()
{
    std::optional<CommandRequest> result = mPendingRequest;
    mPendingRequest.reset();
    return result;
}

TextBuffer CommandProcessor::getOutput() const
{
    std::lock_guard<std::mutex> lock(mOutputMutex);
    return mOutput;
}

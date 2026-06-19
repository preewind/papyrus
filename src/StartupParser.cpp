#include "StartupParser.h"

#include "Argparser.h"

#include <format>
#include <unordered_set>

StartupOptions parseStartupOptions(int argc, char *argv[])
{
    StartupOptions options;
    ArgParser parser;
    const ArgParseResult parsed = parser.parse(argc, argv);

    static const std::unordered_set<std::string> allowedFlags = {"h", "help"};
    const auto formatOption = [](const std::string &flag) -> std::string
    {
        if (flag.size() == 1)
        {
            return std::format("-{}", flag);
        }
        return std::format("--{}", flag);
    };

    for (const auto &[flag, value] : parsed.flags)
    {
        if (!allowedFlags.contains(flag))
        {
            options.valid = false;
            options.exitCode = 2;
            options.errorMessage = std::format("Unknown option '{}'", formatOption(flag));
            return options;
        }

        if ((flag == "h" || flag == "help") && value.has_value())
        {
            options.valid = false;
            options.exitCode = 2;
            options.errorMessage = std::format("Option '{}' does not take a value", formatOption(flag));
            return options;
        }
    }

    if (parsed.hasFlag("h") || parsed.hasFlag("help"))
    {
        options.showHelp = true;
        options.exitCode = 0;
        return options;
    }

    if (parsed.positional.size() > 1)
    {
        options.valid = false;
        options.exitCode = 2;
        options.errorMessage = "Only one filename positional argument is supported";
        return options;
    }

    if (auto firstPositional = parsed.firstPositional())
    {
        options.filename = *firstPositional;
    }

    return options;
}

std::string startupUsageText()
{
    return "usage: papyrus [filename] [option]\n"
           "-h, --help: displays this help message\n";
}

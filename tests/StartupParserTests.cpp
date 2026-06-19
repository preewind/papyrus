#include <gtest/gtest.h>

#include "../src/StartupParser.h"

#include <string>
#include <vector>

namespace
{
std::vector<char *> makeArgv(std::vector<std::string> &args)
{
    std::vector<char *> argv;
    argv.reserve(args.size());
    for (std::string &arg : args)
    {
        argv.push_back(arg.data());
    }
    return argv;
}

StartupOptions parseFrom(const std::vector<std::string> &input)
{
    std::vector<std::string> args = input;
    std::vector<char *> argv = makeArgv(args);
    return parseStartupOptions(static_cast<int>(argv.size()), argv.data());
}
} // namespace

TEST(StartupParser, DefaultInvocationIsValid)
{
    const StartupOptions options = parseFrom({"papyrus"});

    EXPECT_TRUE(options.valid);
    EXPECT_FALSE(options.showHelp);
    EXPECT_TRUE(options.filename.empty());
    EXPECT_EQ(options.exitCode, 0);
    EXPECT_TRUE(options.errorMessage.empty());
}

TEST(StartupParser, ParsesFilename)
{
    const StartupOptions options = parseFrom({"papyrus", "notes.txt"});

    EXPECT_TRUE(options.valid);
    EXPECT_FALSE(options.showHelp);
    EXPECT_EQ(options.filename, "notes.txt");
    EXPECT_EQ(options.exitCode, 0);
}

TEST(StartupParser, HelpShortFlag)
{
    const StartupOptions options = parseFrom({"papyrus", "-h"});

    EXPECT_TRUE(options.valid);
    EXPECT_TRUE(options.showHelp);
    EXPECT_EQ(options.exitCode, 0);
}

TEST(StartupParser, HelpLongFlag)
{
    const StartupOptions options = parseFrom({"papyrus", "--help"});

    EXPECT_TRUE(options.valid);
    EXPECT_TRUE(options.showHelp);
    EXPECT_EQ(options.exitCode, 0);
}

TEST(StartupParser, RejectsUnknownLongOption)
{
    const StartupOptions options = parseFrom({"papyrus", "--bogus"});

    EXPECT_FALSE(options.valid);
    EXPECT_FALSE(options.showHelp);
    EXPECT_EQ(options.exitCode, 2);
    EXPECT_EQ(options.errorMessage, "Unknown option '--bogus'");
}

TEST(StartupParser, RejectsUnknownShortOption)
{
    const StartupOptions options = parseFrom({"papyrus", "-x"});

    EXPECT_FALSE(options.valid);
    EXPECT_FALSE(options.showHelp);
    EXPECT_EQ(options.exitCode, 2);
    EXPECT_EQ(options.errorMessage, "Unknown option '-x'");
}

TEST(StartupParser, RejectsTooManyPositionalArguments)
{
    const StartupOptions options = parseFrom({"papyrus", "a.txt", "b.txt"});

    EXPECT_FALSE(options.valid);
    EXPECT_EQ(options.exitCode, 2);
    EXPECT_EQ(options.errorMessage, "Only one filename positional argument is supported");
}

TEST(StartupParser, RejectsHelpFlagWithValueUsingEquals)
{
    const StartupOptions options = parseFrom({"papyrus", "--help=true"});

    EXPECT_FALSE(options.valid);
    EXPECT_EQ(options.exitCode, 2);
    EXPECT_EQ(options.errorMessage, "Option '--help' does not take a value");
}

TEST(StartupParser, RejectsHelpFlagWithValueAsNextToken)
{
    const StartupOptions options = parseFrom({"papyrus", "-h", "true"});

    EXPECT_FALSE(options.valid);
    EXPECT_EQ(options.exitCode, 2);
    EXPECT_EQ(options.errorMessage, "Option '-h' does not take a value");
}

TEST(StartupParser, RejectsUnknownOptionInShortBundle)
{
    const StartupOptions options = parseFrom({"papyrus", "-xh"});

    EXPECT_FALSE(options.valid);
    EXPECT_EQ(options.exitCode, 2);
    EXPECT_EQ(options.errorMessage, "Unknown option '-x'");
}

TEST(StartupParser, UsageTextContainsSynopsisAndHelp)
{
    const std::string usage = startupUsageText();

    EXPECT_NE(usage.find("usage: papyrus [filename] [option]"), std::string::npos);
    EXPECT_NE(usage.find("-h, --help: displays this help message"), std::string::npos);
}

#include <gtest/gtest.h>

#include "Argparser.h"

#include <optional>
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
} // namespace

TEST(ArgParser, ParsesNoArguments)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(result.executableName, "papyrus");
    EXPECT_TRUE(result.flags.empty());
    EXPECT_TRUE(result.positional.empty());
    EXPECT_FALSE(result.firstPositional().has_value());
}

TEST(ArgParser, ParsesSinglePositional)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "notes.txt"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    ASSERT_EQ(result.positional.size(), 1u);
    EXPECT_EQ(result.positional[0], "notes.txt");
    EXPECT_EQ(result.firstPositional(), std::optional<std::string>("notes.txt"));
}

TEST(ArgParser, ParsesShortBooleanFlag)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-h"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("h"));
    EXPECT_EQ(result.getFlagValue("h"), std::nullopt);
}

TEST(ArgParser, ParsesLongBooleanFlag)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "--help"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("help"));
    EXPECT_EQ(result.getFlagValue("help"), std::nullopt);
}

TEST(ArgParser, ParsesShortFlagWithValue)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-o", "out.txt"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("o"));
    EXPECT_EQ(result.getFlagValue("o"), std::optional<std::string>("out.txt"));
}

TEST(ArgParser, ParsesLongFlagWithValue)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "--theme", "solarized"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("theme"));
    EXPECT_EQ(result.getFlagValue("theme"), std::optional<std::string>("solarized"));
}

TEST(ArgParser, ParsesLongFlagEqualsValue)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "--theme=solarized"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("theme"));
    EXPECT_EQ(result.getFlagValue("theme"), std::optional<std::string>("solarized"));
}

TEST(ArgParser, ParsesShortFlagBundle)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-abc"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("a"));
    EXPECT_TRUE(result.hasFlag("b"));
    EXPECT_TRUE(result.hasFlag("c"));
    EXPECT_EQ(result.getFlagValue("a"), std::nullopt);
    EXPECT_EQ(result.getFlagValue("b"), std::nullopt);
    EXPECT_EQ(result.getFlagValue("c"), std::nullopt);
}

TEST(ArgParser, ParsesDashDashTerminator)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "--", "--help", "file.txt"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_FALSE(result.hasFlag("help"));
    ASSERT_EQ(result.positional.size(), 2u);
    EXPECT_EQ(result.positional[0], "--help");
    EXPECT_EQ(result.positional[1], "file.txt");
}

TEST(ArgParser, ParsesSingleDashAsPositional)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    ASSERT_EQ(result.positional.size(), 1u);
    EXPECT_EQ(result.positional[0], "-");
}

TEST(ArgParser, MissingFlagValueReturnsNullopt)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-h"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(result.getFlagValue("does-not-exist"), std::nullopt);
}

TEST(ArgParser, TrailingFlagWithMissingValue)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-o"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("o"));
    EXPECT_EQ(result.getFlagValue("o"), std::nullopt);
}

TEST(ArgParser, TrailingLongFlagWithMissingValue)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "--theme"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("theme"));
    EXPECT_EQ(result.getFlagValue("theme"), std::nullopt);
}

TEST(ArgParser, LongFlagWithEmptyEqualsValue)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "--theme="};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("theme"));
    EXPECT_EQ(result.getFlagValue("theme"), std::optional<std::string>(""));
}

TEST(ArgParser, HandlesNullPointersInArgv)
{
    ArgParser parser;
    // Constructing a manual argv array containing an explicit inner nullptr
    std::string exec = "papyrus";
    std::string pos = "file.txt";
    std::vector<char *> argv = { exec.data(), nullptr, pos.data() };

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(result.executableName, "papyrus");
    ASSERT_EQ(result.positional.size(), 1u);
    EXPECT_EQ(result.positional[0], "file.txt");
}

TEST(ArgParser, HandlesZeroArgc)
{
    ArgParser parser;
    const ArgParseResult result = parser.parse(0, nullptr);

    EXPECT_TRUE(result.executableName.empty());
    EXPECT_TRUE(result.flags.empty());
    EXPECT_TRUE(result.positional.empty());
}

TEST(ArgParser, ParsesEmptyStringsAsPositional)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "", "file.txt"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    ASSERT_EQ(result.positional.size(), 2u);
    EXPECT_EQ(result.positional[0], "");
    EXPECT_EQ(result.positional[1], "file.txt");
}

TEST(ArgParser, TripleDashFlag)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "---"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    // token.substr(2) results in "-"
    EXPECT_TRUE(result.hasFlag("-"));
    EXPECT_EQ(result.getFlagValue("-"), std::nullopt);
}

TEST(ArgParser, DuplicateFlagsOverwrite)
{
    ArgParser parser;
    std::vector<std::string> args = {"papyrus", "-v", "1", "-v", "2"};
    std::vector<char *> argv = makeArgv(args);

    const ArgParseResult result = parser.parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result.hasFlag("v"));
    EXPECT_EQ(result.getFlagValue("v"), std::optional<std::string>("2"));
}

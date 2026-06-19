#include <gtest/gtest.h>

#include "../src/Argparser.h"

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

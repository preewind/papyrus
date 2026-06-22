#include <gtest/gtest.h>
#include "Application.h"
#include <vector>
#include <string>

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

TEST(ApplicationTest, ExitsEarlyWhenHelpRequested)
{
    // Assuming "--help" sets startup.showHelp = true and startup.exitCode = 0
    std::vector<std::string> args = {"papyrus", "--help"};
    std::vector<char *> argv = makeArgv(args);

    Application app(static_cast<int>(argv.size()), argv.data());

    // The application should not be running and should return a clean exit code
    EXPECT_EQ(app.exitCode(), 0);
}

// Test path where user inputs an invalid command-line flag
TEST(ApplicationTest, ExitsWithFailureOnInvalidArguments)
{
    std::vector<std::string> args = {"papyrus", "--completely-invalid-flag"};
    std::vector<char *> argv = makeArgv(args);

    Application app(static_cast<int>(argv.size()), argv.data());

    EXPECT_NE(app.exitCode(), 0);
}

#include <gtest/gtest.h>

#include "logger.h"

#include <sstream>
#include <string>

namespace
{
class StreamRedirector
{
public:
    StreamRedirector(std::ostream& originalStream, std::stringstream& targetBuffer)
        : mOriginalStream(originalStream), mOriginalBuffer(originalStream.rdbuf(targetBuffer.rdbuf()))
    {
    }

    ~StreamRedirector()
    {
        mOriginalStream.rdbuf(mOriginalBuffer);
    }

private:
    std::ostream& mOriginalStream;
    std::streambuf* mOriginalBuffer;
};
}

TEST(LoggerTests, InfoLogGoesToCoutWithCorrectFormat)
{
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cout, buffer);
        LOG_INFO() << "Application started successfully";
    } // Logger and Redirector are destroyed here, flushing the output

    const std::string output = buffer.str();

    EXPECT_NE(output.find("[INFO]"), std::string::npos);
    EXPECT_NE(output.find("Application started successfully"), std::string::npos);
    EXPECT_NE(output.find("LoggerTests.cpp"), std::string::npos);
    EXPECT_EQ(output.back(), '\n');
}

TEST(LoggerTests, ErrorLogGoesToCerrWithCorrectFormat)
{
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cerr, buffer);
        LOG_ERROR() << "Database connection failed";
    }

    const std::string output = buffer.str();

    EXPECT_NE(output.find("[ERROR]"), std::string::npos);
    EXPECT_NE(output.find("Database connection failed"), std::string::npos);
    EXPECT_EQ(output.back(), '\n');
}

TEST(LoggerTests, SupportsChainingMultipleTypes)
{
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cout, buffer);
        LOG_WARN() << "Process code " << 404 << " occurred at latency " << 1.25 << "s";
    }

    const std::string output = buffer.str();

    EXPECT_NE(output.find("[WARNING]"), std::string::npos);
    EXPECT_NE(output.find("Process code 404 occurred at latency 1.25s"), std::string::npos);
}

TEST(LoggerTests, PathParsingExtractsParentAndFileName)
{
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cout, buffer);
        
        // Directly testing the Logger constructor to control the file path string
        Logger(LogLevel::DEBUG, "src/core/Engine.cpp", "Init", 42) << "Booting engine";
    }

    const std::string output = buffer.str();

    EXPECT_NE(output.find("[DEBUG]"), std::string::npos);
    // Verifies your cleanPath logic evaluates to "core/Engine.cpp"
    EXPECT_NE(output.find("[core/Engine.cpp:Init:42]"), std::string::npos);
}

TEST(LoggerTests, PathParsingHandlesFlatFileName)
{
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cout, buffer);
        
        // Directly testing the constructor with no parent directory paths
        Logger(LogLevel::INFO, "main.cpp", "main", 1) << "Entry point";
    }

    const std::string output = buffer.str();

    // Verifies your cleanPath logic handles paths without a parent correctly
    EXPECT_NE(output.find("[main.cpp:main:1]"), std::string::npos);
}
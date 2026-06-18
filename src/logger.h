#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>

enum class LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger
{
public:
    Logger(LogLevel level, const std::string &file, const std::string &func, int line)
        : mLevel(level)
    {
        std::filesystem::path path{file};
        std::string cleanPath;
        if (path.has_parent_path())
        {
            cleanPath = path.parent_path().filename().string() + "/" + path.filename().string();
        }
        else
        {
            cleanPath = path.filename().string();
        }
        std::cout << "[" << levelToString(level) << "] "
                  << "[" << cleanPath << ":" << func << ":" << line << "]: ";
    }

    // This destructor runs at the very end of the macro statement,
    // appending a final newline and flushing the stream cleanly.
    ~Logger()
    {
        std::cout << mStream.str() << "\n";
    }

    // Overload so we can chain variables into our log
    template <typename T>
    Logger &operator<<(const T &message)
    {
        mStream << message;
        return *this;
    }

private:
    LogLevel mLevel;
    std::stringstream mStream;

    std::string levelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        }
        return "UNKNOWN";
    }
};

#define LOG_DEBUG() Logger(LogLevel::DEBUG, __FILE__, __FUNCTION__, __LINE__)
#define LOG_INFO() Logger(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__)
#define LOG_WARN() Logger(LogLevel::WARNING, __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR() Logger(LogLevel::ERROR, __FILE__, __FUNCTION__, __LINE__)
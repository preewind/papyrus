#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <format>
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
        : mLevel(level), mOutput(getSinkForLevel(level))
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
        mOutput << "[" << levelToString(level) << "] "
                  << "[" << cleanPath << ":" << func << ":" << line << "]: ";
    }

    ~Logger()
    {
        mOutput << "\n";
    }

    template <typename T>
    Logger &operator<<(const T &message)
    {
        mOutput << message;
        return *this;
    }

private:
    LogLevel mLevel;
    std::ostream &mOutput;

    static std::ostream &getSinkForLevel(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::ERROR:
            return std::cerr;
        case LogLevel::WARNING:
        case LogLevel::INFO:
        case LogLevel::DEBUG:
        default:
            return std::cout;
        }
    }

    const std::string levelToString(LogLevel level) const
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
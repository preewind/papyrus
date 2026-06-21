#pragma once

#include <string>

struct StartupOptions
{
    bool showHelp = false;
    bool valid = true;
    int exitCode = 0;
    std::string filename;
    std::string errorMessage;
};

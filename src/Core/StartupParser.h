#pragma once

#include "StartupOptions.h"

#include <string>

StartupOptions parseStartupOptions(int argc, char *argv[]);
std::string startupUsageText();

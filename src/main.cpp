#include <iostream>
#include <vector>
#include <string>
#include <format>

#include <SDL3/SDL.h>

#include "Application.h"

int main(int argc, char *argv[])
{
    Application app{argc, argv};
    app.run();
    return app.exitCode();
}
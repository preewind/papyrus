#pragma once

#include <memory>

#include "Renderer.h"
#include "Editor.h"
#include "FileBrowser.h"
#include "types.h"

class Renderer;
class Editor;
class FileBrowser;

class Application
{

public:
    Application(int argc, char *argv[]);
    ~Application();
    void run();
    void update();
    void handleHash(SDL_Keymod mod);

private:
    SDL_Window *mWindow;
    Editor mEditor;
    std::unique_ptr<Renderer> mRenderer;
    FileBrowser mFileBrowser;
    Screen mCurrentScreen = Screen::Editor;
};
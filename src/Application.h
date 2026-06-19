#pragma once

#include <memory>

#include "Renderer.h"
#include "Editor.h"
#include "FileBrowser.h"
#include "types.h"
#include "EditorViewPort.h"
#include "EditorView.h"
#include "SearchView.h"
#include "TerminalView.h"

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
    LayoutManager mLayoutManager;
    EditorViewport mEditorViewPort;
    std::unique_ptr<Renderer> mRenderer;
    TextLayout mTextLayout;
    EditorView mEditorView;
    SearchView mSearchView;
    TerminalView mTerminalView;
    FileBrowser mFileBrowser;
    Screen mCurrentScreen = Screen::Editor;
    bool mRunning = true;
};
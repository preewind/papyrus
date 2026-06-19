#pragma once

#include <memory>

#include "CursorBlinker.h"
#include "Renderer.h"
#include "Editor.h"
#include "FileBrowser.h"
#include "types.h"
#include "EditorViewPort.h"
#include "SearchViewPort.h"
#include "EditorView.h"
#include "FileBrowserView.h"
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
    void increaseFontSize();
    void decreaseFontSize();

    SDL_Window *mWindow;
    Editor mEditor;
    LayoutManager mLayoutManager;
    EditorViewport mEditorViewPort;
    SearchViewport mSearchViewPort;
    std::unique_ptr<Renderer> mRenderer;
    TextLayout mTextLayout;
    CursorBlinker mCursorBlinker;
    uint8_t mFontSize = 20;
    EditorView mEditorView;
    FileBrowserView mFileBrowserView;
    SearchView mSearchView;
    TerminalView mTerminalView;
    FileBrowser mFileBrowser;
    Screen mCurrentScreen = Screen::Editor;
    bool mRunning = true;
};
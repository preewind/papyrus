#pragma once

#include <memory>
#include <string>

#include "CursorBlinker.h"
#include "Editor.h"
#include "FileBrowser.h"
#include "IRenderBackend.h"
#include "Renderer.h"
#include "types.h"
#include "EditorViewPort.h"
#include "SearchViewPort.h"
#include "EditorView.h"
#include "FileBrowserView.h"
#include "SearchView.h"
#include "TerminalView.h"
#include "StartupOptions.h"

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
    int exitCode() const;

private:
    void printUsage() const;
    void initializeWindowAndRendering();
    void openInitialFileIfProvided(const std::string &filename);
    void processEvents();
    void handleEvent(const SDL_Event &event);
    void handleGlobalKeyDown(const SDL_KeyboardEvent &keyEvent);
    void handleWindowResized();
    void handleHash(SDL_Keymod mod);
    void updateEditorScreen();
    void updateFileBrowserScreen();
    void updateWindowTitle(const std::string &title);
    void increaseFontSize();
    void decreaseFontSize();

    SDL_Window *mWindow = nullptr;
    Editor mEditor;
    LayoutManager mLayoutManager;
    EditorViewport mEditorViewPort;
    SearchViewport mSearchViewPort;
    std::unique_ptr<IRenderBackend> mRenderBackend;
    std::unique_ptr<Renderer> mRenderer;
    TextLayout mTextLayout;
    CursorBlinker mCursorBlinker;
    uint8_t mFontSize = 20;
    Theme mTheme;
    EditorView mEditorView;
    FileBrowserView mFileBrowserView;
    SearchView mSearchView;
    TerminalView mTerminalView;
    FileBrowser mFileBrowser;
    Screen mCurrentScreen = Screen::Editor;
    bool mRunning = true;
    int mExitCode = 0;
};
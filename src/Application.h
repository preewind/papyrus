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

class Application
{

public:
    Application(int argc, char *argv[]);
    ~Application();
    void run();
    int exitCode() const;

private:
    void update();
    void printUsage() const;
    void initializeWindowAndRendering();
    void openInitialFileIfProvided(const std::string &filename);
    void processEvents();
    void handleEvent(const SDL_Event &event);
    void handleGlobalKeyDown(const SDL_KeyboardEvent &keyEvent);
    void handleHash(SDL_Keymod mod);
    void registerCommands();
    void processTerminalInputResponses();
    void updateEditorScreen();
    void updateFileBrowserScreen();
    void updateWindowTitle(const std::string &title);
    void increaseFontSize();
    void decreaseFontSize();

    SDL_Window *mWindow = nullptr;
    std::unique_ptr<IRenderBackend> mRenderBackend;
    std::unique_ptr<Renderer> mRenderer;
    Theme mTheme;
    uint8_t mFontSize = 20;

    // Layout
    LayoutManager mLayoutManager;
    EditorViewport mEditorViewPort;
    SearchViewport mSearchViewPort;
    TextLayout mTextLayout;

    Editor mEditor;
    FileBrowser mFileBrowser;

    EditorView mEditorView;
    FileBrowserView mFileBrowserView;
    SearchView mSearchView;
    TerminalView mTerminalView;

    CursorBlinker mCursorBlinker;
    Screen mCurrentScreen = Screen::Editor;
    bool mRunning = true;
    int mExitCode = 0;
};
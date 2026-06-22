#include "Application.h"
#include "SDLRenderBackend.h"
#include "StartupParser.h"
#include "logger.h"
#include "util.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

Application::Application(int argc, char *argv[])
{
    const StartupOptions startup = parseStartupOptions(argc, argv);
    if (startup.showHelp)
    {
        printUsage();
        mRunning = false;
        mExitCode = startup.exitCode;
        return;
    }

    if (!startup.valid)
    {
        LOG_ERROR() << "Error: " << startup.errorMessage << "\n\n";
        printUsage();
        mRunning = false;
        mExitCode = startup.exitCode;
        return;
    }

    initializeWindowAndRendering();
    openInitialFileIfProvided(startup.filename);
}

void Application::initializeWindowAndRendering()
{
    CSF(SDL_Init(SDL_INIT_VIDEO));
    mWindow = SDL_CreateWindow("papyrus", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    CSP(mWindow);
    CSF(SDL_StartTextInput(mWindow));
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(mWindow, &windowWidth, &windowHeight);

    mRenderBackend = std::make_unique<SDLRenderBackend>(mWindow, "assets/JetBrainsMono-Regular.ttf", mFontSize);
    mRenderer = std::make_unique<Renderer>(*mRenderBackend, mTheme, static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight));

    mTextLayout.setMeasurer(mRenderBackend.get());
}

void Application::openInitialFileIfProvided(const std::string &filename)
{
    if (!filename.empty() && mEditor.loadFile(filename))
    {
        updateWindowTitle(std::format("papyrus [{}]", filename));
    }
}

void Application::run()
{
    while (mRunning)
    {
        processEvents();
        update();
    }
}

void Application::update()
{
    switch (mCurrentScreen)
    {
    case Screen::Editor:
        updateEditorScreen();
        break;
    case Screen::FileBrowser:
        updateFileBrowserScreen();
        break;
    default:
        LOG_ERROR() << "Unknown Screen!";
        break;
    }
}

void Application::processEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        handleEvent(event);
    }
}

void Application::handleEvent(const SDL_Event &event)
{
    if (mCurrentScreen == Screen::Editor)
    {
        mEditor.handlePaneKeyHandler(event);
    }
    else if (mCurrentScreen == Screen::FileBrowser)
    {
        mFileBrowser.handleKey(event);
    }

    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        handleGlobalKeyDown(event.key);
    }
    else if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
        mRenderer->onResize(event.window.data1, event.window.data2);
    }
    else if (event.type == SDL_EVENT_QUIT)
    {
        mRunning = false;
    }
}

void Application::handleGlobalKeyDown(const SDL_KeyboardEvent &keyEvent)
{
    const SDL_Keycode key = keyEvent.key;
    const SDL_Keymod mod = keyEvent.mod;

    switch (key)
    {
    case SDLK_F3:
        mCurrentScreen = Screen::FileBrowser;
        break;
    case SDLK_F4:
        mCurrentScreen = Screen::Editor;
        break;
    case SDLK_T:
        mEditor.handleT(mod);
        break;
    case SDLK_PLUS:
        if (mod & SDL_KMOD_CTRL)
        {
            increaseFontSize();
        }
        break;
    case SDLK_MINUS:
        if (mod & SDL_KMOD_CTRL)
        {
            decreaseFontSize();
        }
        break;
    case SDLK_HASH:
        handleHash(mod);
        break;
    }
}

void Application::processEditorCommandRequests()
{
    while (auto request = mEditor.getTerminal().consumeRequest())
    {
        EditorCommandActions actions{
            .openFileRequest = [this](const std::filesystem::path &path)
            { mEditor.loadFile(path); },
            .saveCurrentFile = [this]()
            { mEditor.saveFile(); },
            .setEditorLanguage = [this](Language language)
            { mEditor.setLanguage(language); },
            .refreshEditorTokens = [this]()
            { mEditor.updateTokens(); },
            .reportEditorError = [](const std::string &message)
            { LOG_ERROR() << "Error: " << message; }};

        if (auto appRequest = mEditorCommandHandler.handle(*request, actions))
        {
            if (appRequest->type == CommandRequestType::Quit)
            {
                mRunning = false;
            }
        }
    }
}

void Application::updateEditorScreen()
{
    mRenderer->clear();
    processEditorCommandRequests();
    if (mEditor.consumeActivity())
    {
        mCursorBlinker.reset();
    }
    mCursorBlinker.update();
    mLayoutManager.update(mRenderer->getWindowProperties(), mEditor.isTerminalVisible());

    mEditor.updateViewPort(mLayoutManager, mLayoutManager.getLayoutInput().lineHeight);
    mEditorViewPort.updateHorizontal(mEditor, mTextLayout, mLayoutManager.getLayoutConfig(), mLayoutManager.getLayoutInput());
    mEditorViewPort.updateVertical(mEditor, mEditor.getVisibleRows());
    if (mEditor.isSearchActive())
    {
        mSearchViewPort.updateHorizontal(mEditor.getSearch(), mTextLayout, mLayoutManager.getSearchLayout());
    }

    const bool cursorVisible = mCursorBlinker.visible();
    mEditorView.render(*mRenderer, mEditor, mEditorViewPort, mTextLayout, mLayoutManager.getLayoutConfig(), mLayoutManager.getEditorLayout(), cursorVisible);
    mSearchView.render(*mRenderer, mEditor, mTextLayout, mLayoutManager.getSearchLayout(), mSearchViewPort, cursorVisible);
    mTerminalView.render(*mRenderer, mEditor, mTextLayout, mLayoutManager.getTerminalLayout(), mRenderer->getWindowProperties());
    mRenderer->present();
}

void Application::updateFileBrowserScreen()
{
    mLayoutManager.update(mRenderer->getWindowProperties(), false);
    mRenderer->clear();
    mFileBrowserView.render(*mRenderer, mFileBrowser, mTextLayout, mLayoutManager.getFileBrowserLayout());
    mRenderer->present();

    if (auto file = mFileBrowser.consumeOpenRequest())
    {
        mEditor.loadFile(*file);
        updateWindowTitle(std::format("papyrus [{}]", file->filename().string()));
        mCurrentScreen = Screen::Editor;
    }
}

void Application::updateWindowTitle(const std::string &title)
{
    CSF(SDL_SetWindowTitle(mWindow, title.c_str()));
}

void Application::handleHash(SDL_Keymod mod)
{
    bool ctrlHeld = mod & SDL_KMOD_CTRL;
    bool shiftHeld = mod & SDL_KMOD_SHIFT;

    if (ctrlHeld && shiftHeld)
    {
        mEditor.switchFocus();
    }
}

void Application::increaseFontSize()
{
    if(mFontSize > 150){
        return;
    }
    ++mFontSize;
    mRenderer->setFontSize(mFontSize);
}

void Application::decreaseFontSize()
{
    if (mFontSize <= 10)
    {
        return;
    }

    --mFontSize;
    mRenderer->setFontSize(mFontSize);
}

void Application::printUsage() const
{
    LOG_INFO() << startupUsageText();
}

int Application::exitCode() const
{
    return mExitCode;
}

Application::~Application()
{
    if (mWindow != nullptr)
    {
        CSF(SDL_StopTextInput(mWindow));
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }
}

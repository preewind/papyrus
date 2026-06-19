#include "Application.h"
#include "Argparser.h"
#include "SDLRenderBackend.h"
#include "logger.h"
#include "util.h"

Application::Application(int argc, char *argv[])
{
    ArgParser parser;

    parser.parse(argc, argv);
    std::string filename = "";
    Argument filenameArg = Argument(ArgumentType::POSITIONAL, "positional");
    if (parser.hasArgument(filenameArg))
    {
        filename = parser.getArgumentValue(filenameArg);
    }
    Argument helpArg = Argument(ArgumentType::BOOL_FLAG, "h");
    if (parser.hasArgument(helpArg))
    {
        std::cout << "usage: papyrus [filename] [option] \n"
                  << "-h: displays this help message \n";
    }

    SDL_Init(SDL_INIT_VIDEO);

    mWindow = SDL_CreateWindow("papyrus", 1280, 720, SDL_WINDOW_RESIZABLE);
    CSF(SDL_StartTextInput(mWindow));

    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(mWindow, &windowWidth, &windowHeight);

    mRenderBackend = std::make_unique<SDLRenderBackend>(mWindow, "assets/JetBrainsMono-Regular.ttf", mFontSize);
    mRenderer = std::make_unique<Renderer>(*mRenderBackend, mTheme, static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight));

    mTextLayout.setMeasurer(mRenderBackend.get());

    if (!filename.empty())
    {
        mEditor.loadFile(filename);
        SDL_SetWindowTitle(mWindow, std::format("papyrus [{}]", filename).c_str());
    }
}

Application::~Application()
{
    CSF(SDL_StopTextInput(mWindow));
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Application::run()
{

    while (mRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (mCurrentScreen == Screen::Editor)
            {
                mEditor.handlePaneKeyHandler(event);
            }
            else if (mCurrentScreen == Screen::FileBrowser)
            {
                mFileBrowser.handleKey(event);
            }

            // global key bindings
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                SDL_Keycode key = event.key.key;
                SDL_Keymod mod = event.key.mod;
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
                    handleHash(event.key.mod);
                    break;
                }
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                uint32_t w, h;
                SDL_GetWindowSize(mWindow, (int *)&w, (int *)&h);
                mRenderer->onResize(w, h);
            }
            if (event.type == SDL_EVENT_QUIT)
            {
                mRunning = false;
            }
        }
        update();
    }
}

void Application::update()
{
    switch (mCurrentScreen)
    {
    case Screen::Editor:
    {
        if (auto request = mEditor.consumeRequest())
        {
            CommandRequest req = *request;
            switch (req.type)
            {
            case CommandRequestType::Quit:
                mRunning = false;
                break;

            default:
                break;
            }
        }
        mRenderer->clear();
        mEditor.update();
        if (mEditor.consumeActivity())
        {
            mCursorBlinker.reset();
        }
        mCursorBlinker.update();
        mLayoutManager.update(mRenderer->getSDL_Properties(), mEditor.isTerminalVisible());

        mEditor.updateViewPort(mLayoutManager, mLayoutManager.getLayoutInput().lineHeight);
        mEditorViewPort.updateHorizontal(mEditor, mTextLayout, mLayoutManager.getLayoutConfig(), mLayoutManager.getLayoutInput());
        if (mEditor.isSearchActive())
        {
            mSearchViewPort.updateHorizontal(mEditor.getSearch(), mTextLayout, mLayoutManager.getSearchLayout());
        }

        bool cursorVisible = mCursorBlinker.visible();
        mEditorView.render(*mRenderer, mEditor, mEditorViewPort, mTextLayout, mLayoutManager.getLayoutConfig(), mLayoutManager.getEditorLayout(), cursorVisible);
        mSearchView.render(*mRenderer, mEditor, mTextLayout, mLayoutManager.getSearchLayout(), mSearchViewPort, cursorVisible);
        mTerminalView.render(*mRenderer, mEditor, mTextLayout, mLayoutManager.getTerminalLayout(), mRenderer->getSDL_Properties());
        mRenderer->present();
        break;
    }
    case Screen::FileBrowser:
    {
        mLayoutManager.update(mRenderer->getSDL_Properties(), false);
        mRenderer->clear();
        mFileBrowserView.render(*mRenderer, mFileBrowser, mTextLayout, mLayoutManager.getLayoutConfig(), mRenderer->getSDL_Properties());
        mRenderer->present();
        // requests
        if (auto file = mFileBrowser.consumeOpenRequest())
        {
            mEditor.loadFile(*file);
            SDL_SetWindowTitle(mWindow, std::format("papyrus [{}]", file->filename().string()).c_str());
            mCurrentScreen = Screen::Editor;
        }
        break;
    }
    default:
        LOG_ERROR() << "Unknown Screen!";
        break;
    }
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
    ++mFontSize;
    mRenderer->setFontSize(mFontSize);
}

void Application::decreaseFontSize()
{
    if (mFontSize <= 1)
    {
        return;
    }

    --mFontSize;
    mRenderer->setFontSize(mFontSize);
}

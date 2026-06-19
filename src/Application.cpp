#include "Application.h"
#include "Argparser.h"
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

    mRenderer = std::make_unique<Renderer>(mWindow);

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
                    mRenderer->handlePlus(mod);
                    break;
                case SDLK_MINUS:
                    mRenderer->handleMinus(mod);
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
        mEditor.update();
        mRenderer->updateLayout(mEditor);
        mRenderer->updateEditor(mEditor);
        break;
    case Screen::FileBrowser:
        mRenderer->updateFileBrowser(mFileBrowser);
        // requests
        if (auto file = mFileBrowser.consumeOpenRequest())
        {
            mEditor.loadFile(*file);
            SDL_SetWindowTitle(mWindow, std::format("papyrus [{}]", file->filename().string()).c_str());
            mCurrentScreen = Screen::Editor;
        }
        break;
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

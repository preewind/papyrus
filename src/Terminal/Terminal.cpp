#include <sstream>
#include <filesystem>

#include "Terminal.h"
#include "util.h"

void Terminal::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        SDL_Keycode key = event.key.key;
        SDL_Keymod mod = event.key.mod;
        switch (key)
        {
        case SDLK_RETURN:
            handleReturn();
            return;
        case SDLK_UP:
            handleUp(mod);
            return;
        case SDLK_DOWN:
            handleDown(mod);
            return;
        case SDLK_ESCAPE:
            // Cancel input request if in AwaitingInput mode
            if (mInputMode == TerminalInputMode::AwaitingInput && mPendingInputRequest)
            {
                mPendingInputResponse = TerminalInputResponse{
                    .success = false,
                    .userInput = "",
                    .selectedIndex = 0
                };
                mInputMode = TerminalInputMode::Normal;
                mPendingInputRequest.reset();
                mInput.clear();
            }
            return;
        default:
            break;
        }
    }
    mInput.handleKey(event);
}

void Terminal::handleReturn()
{
    // Handle terminal input request (editor-initiated input)
    if (mInputMode == TerminalInputMode::AwaitingInput)
    {
        if (!mPendingInputRequest)
            return;
            
        std::string userInput = mInput.getText();
        
        // Handle different input types
        if (mPendingInputRequest->type == TerminalInputType::SelectFromList)
        {
            // User selected an option via arrow keys and Enter
            if (mSelectListIndex < mPendingInputRequest->options.size())
            {
                mPendingInputResponse = TerminalInputResponse{
                    .success = true,
                    .userInput = mPendingInputRequest->options[mSelectListIndex],
                    .selectedIndex = mSelectListIndex
                };
            }
        }
        else
        {
            // For Filename, Confirmation, TextInput types
            mPendingInputResponse = TerminalInputResponse{
                .success = !userInput.empty(),
                .userInput = userInput,
                .selectedIndex = 0
            };
        }
        
        mInputMode = TerminalInputMode::Normal;
        mPendingInputRequest.reset();
        mInput.clear();
        mCmdHistory.push_back(userInput);
        mHistoryIndex = 0;
        return;
    }
    
    if (mInput.getText().empty())
        return;
    std::string trimmedInput = trim(mInput.getText());

    std::string command;
    std::vector<std::string> options;
    if (!trimmedInput.empty() && !trimmedInput.starts_with("!"))
    {
        std::stringstream ss(trimmedInput);
        ss >> command;
        std::string option;
        while (ss >> option)
        {
            options.push_back(option);
        }
        mProcessor.executeCommand(command, options);
    }
    else if (trimmedInput.starts_with("!"))
    {
        mProcessor.executeCommand(trimmedInput, {});
    }
    mCmdHistory.push_back(trimmedInput);
    mInput.clear();
    mHistoryIndex = 0;
}

void Terminal::handleUp(SDL_Keymod mod)
{
    const bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        const auto &outputLines = mProcessor.getOutput().getText();
        uint32_t outputRows = 0;
        for (const std::string &line : outputLines)
        {
            if (!line.empty())
            {
                outputRows++;
            }
        }
        const uint32_t maxOffset = outputRows > mVisibleRows ? outputRows - mVisibleRows : 0;

        if (mScrollOffset < maxOffset)
        {
            mScrollOffset++;
        }
    }
    else
    {
        if (!mCmdHistory.empty() && mHistoryIndex < mCmdHistory.size())
        {
            if (mHistoryIndex == 0 && !mInput.getText().empty())
                mSaveInput = mInput.getText();
            mHistoryIndex++;
            mInput.clear();
            uint32_t targetIndex = mCmdHistory.size() - mHistoryIndex;
            mInput.insert(mCmdHistory[targetIndex]);
        }
    }
}

void Terminal::handleDown(SDL_Keymod mod)
{
    const bool ctrlHeld = mod & SDL_KMOD_CTRL;

    if (ctrlHeld)
    {
        if (mScrollOffset > 0)
        {
            mScrollOffset--;
        }
    }
    else
    {
        if (!mCmdHistory.empty() && mHistoryIndex > 0)
        {
            mInput.clear();
            mHistoryIndex--;
            if (mHistoryIndex == 0)
            {
                mInput.insert(mSaveInput);
            }
            else
            {
                uint32_t targetIndex = mCmdHistory.size() - mHistoryIndex;
                mInput.insert(mCmdHistory[targetIndex]);
            }
        }
    }
}

const std::string& Terminal::getInput() const
{
    return mInput.getText();
}

const TextBuffer& Terminal::getOutput() const
{
    return mProcessor.getOutput();
}

uint32_t Terminal::getCursor() const
{
    return mInput.getCursor();
}

uint32_t Terminal::getScrollOffset() const
{
    return mScrollOffset;
}

uint32_t Terminal::getVisibleRows() const
{
    return mVisibleRows;
}

void Terminal::setVisibleRows(uint32_t rows)
{
    mVisibleRows = rows;
}

bool Terminal::hasSelection() const
{
    return mInput.hasSelection();
}

TextSelection Terminal::getSelection() const
{
    return mInput.getSelection();
}

std::string Terminal::getPromptPrefix() const
{
    if (mInputMode == TerminalInputMode::AwaitingInput && mPendingInputRequest)
    {
        return mPendingInputRequest->prompt;
    }
    return std::filesystem::current_path().string() + "$ ";
}

void Terminal::requestInput(const TerminalInputRequest &request)
{
    mPendingInputRequest = request;
    mInputMode = TerminalInputMode::AwaitingInput;
    mInput.clear();
    if (!request.defaultValue.empty())
    {
        mInput.insert(request.defaultValue);
    }
    mSelectListIndex = 0;
}

std::optional<TerminalInputResponse> Terminal::consumeInputResponse()
{
    auto response = mPendingInputResponse;
    mPendingInputResponse.reset();
    return response;
}

TerminalInputMode Terminal::getInputMode() const
{
    return mInputMode;
}

void Terminal::registerCommand(CommandDefinition def)
{
    mProcessor.registerCommand(std::move(def));
}

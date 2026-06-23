#pragma once

#include <vector>
#include <string>
#include <optional>

#include <SDL3/SDL_events.h>

#include "TextBuffer.h"
#include "TextInput.h"
#include "CommandProcessor.h"
#include "types.h"

enum class TerminalInputMode
{
    Normal,
    AwaitingInput
};

class Terminal
{

public:
    void handleKey(const SDL_Event &event);
    void handleReturn();
    void handleUp(SDL_Keymod mod);
    void handleDown(SDL_Keymod mod);
    const std::string &getInput() const;
    const TextBuffer &getOutput() const;
    uint32_t getCursor() const;
    uint32_t getScrollOffset() const;
    uint32_t getVisibleRows() const;
    void setVisibleRows(uint32_t rows);
    bool hasSelection() const;
    TextSelection getSelection() const;
    std::string getPromptPrefix() const;
    void registerCommand(CommandDefinition def);
    void requestInput(const TerminalInputRequest &request);
    std::optional<TerminalInputResponse> consumeInputResponse();
    TerminalInputMode getInputMode() const;

private:
    TextInput mInput;
    std::vector<std::string> mCmdHistory;
    std::string mSaveInput;
    uint32_t mHistoryIndex = 0;
    uint32_t mScrollOffset = 0;
    uint32_t mVisibleRows = 0;
    CommandProcessor mProcessor;
    
    // Input request/response state
    std::optional<TerminalInputRequest> mPendingInputRequest;
    std::optional<TerminalInputResponse> mPendingInputResponse;
    TerminalInputMode mInputMode = TerminalInputMode::Normal;
    size_t mSelectListIndex = 0;
};
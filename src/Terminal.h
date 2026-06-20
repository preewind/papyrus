#pragma once

#include <vector>
#include <string>
#include <optional>

#include <SDL3/SDL_events.h>

#include "TextBuffer.h"
#include "TextInput.h"
#include "CommandProcessor.h"
#include "types.h"

class Terminal
{

public:
    void handleKey(const SDL_Event &event);
    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleDelete();
    void handleReturn();
    void handleUp(SDL_Keymod mod);
    void handleDown(SDL_Keymod mod);
    void handleRight();
    void handleLeft();
    void handleHome();
    void handleEnd();
    std::string getInput() const;
    TextBuffer getOutput() const;
    uint32_t getCursor() const;
    uint32_t getScrollOffset() const;
    uint32_t getVisibleRows() const;
    void setVisibleRows(uint32_t rows);
    std::optional<CommandRequest> consumeRequest();
    bool hasSelection() const;
    TextSelection getSelection() const;

private:
    TextInput mInput;
    std::vector<std::string> mCmdHistory;
    std::string mSaveInput = "";
    uint32_t mHistoryIndex = 0;
    uint32_t mScrollOffset = 0;
    uint32_t mVisibleRows = 0;
    CommandProcessor mProcessor;
};
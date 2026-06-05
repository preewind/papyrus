#pragma once

#include <vector>
#include <string>

#include <SDL3/SDL_stdinc.h>

#include "Renderer.h"
#include "TextBuffer.h"

struct Cursor {
    size_t row, col;
};

class Editor{

public:
    Editor();
    ~Editor();

    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleReturn();
    void handleLeft();
    void handleRight();
    void handleUp();
    void handleDown();
    void handleTab();
    
    Cursor getCursor() const;
    const std::string& getLineString(int i) const;
    const std::vector<std::string>& getText() const;

private:
    Cursor mCursor;
    TextBuffer mBuffer;
};
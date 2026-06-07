#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include <SDL3/SDL_stdinc.h>

#include "Renderer.h"
#include "TextBuffer.h"

struct Cursor {
    size_t row, col;
};

struct Selection {
    size_t begin_row;
    size_t begin_col;
    size_t end_row;
    size_t end_col;
};

class Editor{

public:
    Editor();
    ~Editor();

    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleReturn();
    void handleLeft(SDL_Keymod mod);
    void handleRight(SDL_Keymod mod);
    void handleUp();
    void handleDown();
    void handleTab();
    void handleShift(Uint32 type);

    void loadFile(const std::filesystem::path& path);
    void saveFileAs(const std::filesystem::path& path);
    void saveFile();

    void markActivity();
    bool consumeActivity();

    const Selection& getSelection() const;
    void setSelectionActive(bool b);
    bool getSelectionActive()const;
    
    Cursor getCursor() const;
    const std::string& getLineString(int i) const;
    const std::vector<std::string>& getText() const;

private:
    Selection mSelection{0};
    Cursor mCursor;
    TextBuffer mBuffer;
    std::filesystem::path mCurrentFilePath;
    bool mActivity;
    bool mSelectionActive = false;
};
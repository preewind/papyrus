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

    void loadFile(const std::filesystem::path& path);
    void saveFileAs(const std::filesystem::path& path);
    void saveFile();

    void markActivity();
    bool consumeActivity();
    
    Cursor getCursor() const;
    const std::string& getLineString(int i) const;
    const std::vector<std::string>& getText() const;

private:
    Cursor mCursor;
    TextBuffer mBuffer;
    std::filesystem::path mCurrentFilePath;
    bool mActivity;
};
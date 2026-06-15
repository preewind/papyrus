#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <optional>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_events.h>

#include "TextBuffer.h"
#include "types.h"
#include "SearchSession.h"
#include "SyntaxHighlighter.h"
#include "Terminal.h"

enum class Focus {
    Editor,
    Terminal
};

class Editor
{

public:
    Editor();
    ~Editor();

    void handlePaneKeyHandler(const SDL_Event &event);
    void handleKey(const SDL_Event &event);
    void handleTextInput(const std::string &text);
    void handleBackSpace();
    void handleReturn();
    void handleTab();
    void handleDelete(SDL_Keymod mod);
    void handleLeft(SDL_Keymod mod);
    void handleRight(SDL_Keymod mod);
    void handleUp(SDL_Keymod mod);
    void handleDown(SDL_Keymod mod);
    void handleHome(SDL_Keymod mod);
    void handleEnd(SDL_Keymod mod);
    void handleComma(SDL_Keymod mod);
    void handleA(SDL_Keymod mod);
    void handleC(SDL_Keymod mod);
    void handleF(SDL_Keymod mod);
    void handleS(SDL_Keymod mod);
    void handleT(SDL_Keymod mod);
    void handleV(SDL_Keymod mod);

    void moveCursorLeft();
    void moveCursorRight();
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorToBeginCol();
    void moveCursorToEndCol();
    void moveCursorToFirstRow();
    void moveCursorToLastRow();

    void ensureCursorVisibleVertically();

    void loadFile(const std::filesystem::path &path);
    void saveFileAs(const std::filesystem::path &path);
    void saveFile();

    bool isSearchActive() const;
    const SearchSession &getSearch() const;
    void updateSearchMatches();

    void updateTokens();
    std::vector<std::vector<Token>> getTokens() const;

    void markActivity();
    bool consumeActivity();

    bool isTerminalVisible() const;
    void switchFocus();
    const Terminal& getTerminal() const;

    const Selection &getSelection() const;
    void setSelectionActive(bool b);
    bool getSelectionActive() const;
    void clearSelection();
    void beginSelection();
    void updateSelection();
    const std::string getSelectedText() const;

    Cursor getCursor() const;
    const std::string &getLineString(int i) const;
    uint32_t getLineCount() const;
    const std::vector<std::string> &getText() const;
    void setVisibleRows(uint32_t rows);
    const uint32_t &getVisibleRows() const;
    const uint32_t &getScrollOffsetY() const;

private:
    Selection mSelection;
    Cursor mCursor;
    TextBuffer mBuffer;
    Focus mFocus = Focus::Editor;
    bool mTerminalVisible = false;
    Terminal mTerminal;
    std::optional<SearchSession> mSearch;
    SearchEngine mSearchEngine;
    std::filesystem::path mCurrentFilePath;
    bool mActivity;
    bool mSelectionActive = false;
    uint32_t mScrollOffsetY = 0;
    uint32_t mVisibleRows = 0;
    uint32_t mVisibleTextWidth = 0;

    Language mLanguage = Language::Cpp;
    std::vector<std::vector<Token>> mTokens;
    SyntaxHighlighter mHighlighter;
};
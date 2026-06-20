#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include <SDL3/SDL_events.h>

#include "SearchEngine.h"
#include "TextInput.h"
#include "types.h"

class SearchSession
{
public:
    Cursor handleKey(const SDL_Event &event, Cursor editorCursor);

    void addToQuery(const std::string &text);
    void addToCursor(uint32_t size);
    void resetCursor();

    std::string getQuery() const;
    uint32_t getCursor() const;
    void setMatches(const std::vector<SearchMatch> &matches);
    std::vector<SearchMatch> getMatches() const;
    uint32_t getCurrentMatchIndex() const;
    bool hasMatches() const;
    bool hasSelection() const;
    TextSelection getSelection() const;
    uint32_t mCurrentMatch = 0;

private:
    Cursor cycleUp(Cursor editorCursor);
    Cursor cycleDown(Cursor editorCursor);

    TextInput mQuery;
    std::vector<SearchMatch> mMatches;
};
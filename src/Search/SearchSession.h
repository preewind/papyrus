#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include <SDL3/SDL_events.h>

#include "TextInput.h"
#include "types.h"

class SearchSession
{
public:
    Cursor handleKey(const SDL_Event &event, Cursor editorCursor);

    const std::string& getQuery() const;
    uint32_t getCursor() const;
    void setMatches(const std::vector<SearchMatch> &matches);
    const std::vector<SearchMatch>& getMatches() const;
    uint32_t getCurrentMatchIndex() const;
    bool hasMatches() const;
    bool hasSelection() const;
    TextSelection getSelection() const;

private:
    Cursor cycleUp(Cursor editorCursor);
    Cursor cycleDown(Cursor editorCursor);

    uint32_t mCurrentMatch = 0;
    TextInput mQuery;
    std::vector<SearchMatch> mMatches;
};
#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "SearchEngine.h"
#include "TextBuffer.h"
#include "types.h"

class SearchSession
{
public:
    std::string getQuery() const;
    void addToQuery(const std::string &text);
    void handleBackSpace();
    void handleDelete();
    void handleLeft();
    void handleRight();
    void handleEnd();
    Cursor handleUp(Cursor &cursor);
    Cursor handleDown(Cursor &cursor);
    void addToCursor(uint32_t size);
    void resetCursor();
    uint32_t getCursor() const;
    void setMatches(const std::vector<SearchMatch> &matches);
    std::vector<SearchMatch> getMatches() const;
    uint32_t getCurrentMatchIndex() const;
    bool hasMatches() const;
    uint32_t mCurrentMatch = 0;

private:
    TextBuffer mQuery;
    std::vector<SearchMatch> mMatches;
    uint32_t mCursor = 0;
};
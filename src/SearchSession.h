#pragma once
#include <string>
#include <vector>
#include <stdint.h>

#include "SearchEngine.h"
#include "types.h"

class TextBuffer;

class SearchSession
{
public:

    std::string getQuery() const;
    void addToQuery(const std::string &text);
    void setMatches(const std::vector<SearchMatch> &matches);
    std::vector<SearchMatch> getMatches() const;
    uint32_t mCurrentMatch = 0;

private:
    std::string mQuery;
    std::vector<SearchMatch> mMatches;
    
};
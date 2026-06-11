#pragma once

#include <vector>

#include "types.h"

class TextBuffer;

class SearchEngine
{
public:
    std::vector<SearchMatch> find(const TextBuffer& buffer, const std::string& query);
};
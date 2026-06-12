#pragma once

#include <vector>

#include "types.h"
#include "TextBuffer.h"
#include "CppLexer.h"

class SyntaxHighlighter
{

public:
    std::vector<std::vector<Token>> tokenize(const TextBuffer &buffer, Language language);

private:
    CppLexer mCppLexer;
};
#pragma once

#include <vector>
#include <string>
#include <unordered_set>

#include "types.h"
#include "TextBuffer.h"

class CppLexer
{
public:
    std::vector<std::vector<Token>> tokenize(const TextBuffer &buffer);
    const std::unordered_set<std::string> importantKeywords = {
        "int", "float", "double", "char", "void", "if", "else", "for", "while", "return",
        "class", "struct", "enum", "union", "namespace", "template", "typename",
        "public", "private", "protected", "virtual", "override", "final"};
};

#include "SyntaxHighlighter.h"

std::vector<std::vector<Token>> SyntaxHighlighter::tokenize(const TextBuffer &buffer, Language language)
{
    std::vector<std::vector<Token>> tokens;
    switch (language)
    {
    case Language::Cpp:
        tokens = mCppLexer.tokenize(buffer);
        break;

    default:
        break;
    }
    return tokens;
}
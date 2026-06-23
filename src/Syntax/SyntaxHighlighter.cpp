#include "SyntaxHighlighter.h"
#include "logger.h"

std::vector<std::vector<Token>> SyntaxHighlighter::tokenize(const TextBuffer &buffer, const Language& language) const
{
    switch (language)
    {
    case Language::Cpp:
        return mCppLexer.tokenize(buffer);
        break;

    default:
        LOG_ERROR() << "This language is currently not supported!, TODO: add default lexer";
        break;
    }
    return std::vector<std::vector<Token>>();
}
#include <string>

#include "CppLexer.h"
#include "logger.h"

std::vector<std::vector<Token>> CppLexer::tokenize(const TextBuffer &buffer)
{

    LOG_DEBUG() << "Hello from Cpp lexer!";
    std::vector<std::vector<Token>> allTokens;
    allTokens.reserve(buffer.getText().size());
    for (const std::string &line : buffer.getText())
    {
        std::vector<Token> tokens;
        if (line.empty())
        {
            allTokens.push_back(tokens);
            continue;
        }
        // LOG_DEBUG() << "Line: " << line;

        uint32_t i = 0;
        while (i < line.size())
        {
            char c = line[i];

            if (std::isspace(c))
            {
                ++i;
                continue;
            }

            uint32_t start = i;
            // single line comments
            if (c == '/' && i + 1 < line.size() && line[i + 1] == '/')
            {
                tokens.push_back(Token{.col = start, .length = (uint32_t)line.size() - start, .type = TokenType::Comment});
                break;
            }
            // preprocessor
            if (c == '#')
            {
                ++i;
                while (i < line.size() && std::isalpha(line[i]))
                {
                    ++i;
                }

                tokens.push_back(Token{.col = start, .length = i - start, .type = TokenType::Preprocessor});
                continue;
            }
            // keywords / identfiers
            if (std::isalpha(c) || c == '_')
            {
                ++i;
                while (i < line.size() && (std::isalnum(line[i]) || line[i] == '_'))
                {
                    ++i;
                }
                const std::string &word = line.substr(start, i - start);

                TokenType type = importantKeywords.contains(word) ? TokenType::Keyword : TokenType::Normal;
                tokens.push_back(Token{.col = start, .length = i - start, .type = type});
                continue;
            }
            // strings
            if (c == '"' || c == '\'')
            {
                char quoteType = c;
                ++i;
                while (i < line.size() && line[i] != quoteType)
                {
                    // Handle escaped quotes like \"
                    if (line[i] == '\\' && i + 1 < line.size())
                    {
                        i += 2;
                    }
                    else
                    {
                        ++i;
                    }
                }
                if (i < line.size())
                    ++i; // because we know it is " or '
                tokens.push_back(Token{.col = start, .length = i - start, .type = TokenType::String});
                continue;
            }
            // numbers
            if (std::isdigit(c))
            {
                ++i;
                while (i < line.size() && (std::isdigit(line[i]) || line[i] == '.'))
                {
                    ++i;
                }
                tokens.push_back(Token{.col = start, .length = i - start, .type = TokenType::Number});
                continue;
            }
            // parentheses
            switch (c)
            {
            case '(':
                tokens.push_back({start, 1, TokenType::OpenParen});
                ++i;
                continue;
            case ')':
                tokens.push_back({start, 1, TokenType::CloseParen});
                ++i;
                continue;
            case '{':
                tokens.push_back({start, 1, TokenType::OpenCurly});
                ++i;
                continue;
            case '}':
                tokens.push_back({start, 1, TokenType::CloseCurly});
                ++i;
                continue;
            }
            // includeLib
            if (c == '<' && !tokens.empty() && tokens.back().type == TokenType::Preprocessor)
            {
                ++i;
                while (i < line.size() && line[i] != '>')
                    ++i;
                if (i < line.size())
                    ++i; // Consume '>'

                tokens.push_back(Token{.col = start, .length = i - start, .type = TokenType::IncludeLib});
                continue;
            }
            // for now just unknown the rest
            tokens.push_back(Token{.col = start, .length = 1, .type = TokenType::Unknown});
            ++i;
        }
        std::stringstream lineTokens;
        for (const Token &token : tokens)
        {
            lineTokens << token.type << " ";
            // LOG_DEBUG() << "Token: " << token.type << " at col " << token.col << " with length " << token.length;
        }
        LOG_DEBUG() << "TokenLine: " << lineTokens.str();
        allTokens.push_back(std::move(tokens));
    }

    return allTokens;
}
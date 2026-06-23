#include <gtest/gtest.h>

#include <vector>

#include "CppLexer.h"
#include "SyntaxHighlighter.h"
#include "TextBuffer.h"

namespace
{
void expectToken(const Token &token, uint32_t col, uint32_t length, TokenType type)
{
    EXPECT_EQ(token.col, col);
    EXPECT_EQ(token.length, length);
    EXPECT_EQ(token.type, type);
}
} // namespace

TEST(CppLexerTests, PreservesLineCountForEmptyAndNonEmptyLines)
{
    TextBuffer buffer(std::vector<std::string>{"", "int x"});
    CppLexer lexer;

    const auto tokensByLine = lexer.tokenize(buffer);

    ASSERT_EQ(tokensByLine.size(), 2u);
    EXPECT_TRUE(tokensByLine[0].empty());
    ASSERT_EQ(tokensByLine[1].size(), 2u);
    expectToken(tokensByLine[1][0], 0u, 3u, TokenType::Keyword);
    expectToken(tokensByLine[1][1], 4u, 1u, TokenType::Normal);
}

TEST(CppLexerTests, RecognizesCommentPreprocessorIncludeAndNumbers)
{
    TextBuffer buffer(std::vector<std::string>{"#include <vector>", "double pi = 3.14; // value"});
    CppLexer lexer;

    const auto tokensByLine = lexer.tokenize(buffer);

    ASSERT_EQ(tokensByLine.size(), 2u);

    ASSERT_EQ(tokensByLine[0].size(), 2u);
    expectToken(tokensByLine[0][0], 0u, 8u, TokenType::Preprocessor);
    expectToken(tokensByLine[0][1], 9u, 8u, TokenType::IncludeLib);

    ASSERT_EQ(tokensByLine[1].size(), 6u);
    expectToken(tokensByLine[1][0], 0u, 6u, TokenType::Keyword);
    expectToken(tokensByLine[1][1], 7u, 2u, TokenType::Normal);
    expectToken(tokensByLine[1][2], 10u, 1u, TokenType::Unknown);
    expectToken(tokensByLine[1][3], 12u, 4u, TokenType::Number);
    expectToken(tokensByLine[1][4], 16u, 1u, TokenType::Unknown);
    expectToken(tokensByLine[1][5], 18u, 8u, TokenType::Comment);
}

TEST(CppLexerTests, RecognizesStringsAndBraces)
{
    TextBuffer buffer(std::vector<std::string>{"if (x) {", "char c = '\\'';", "std::string s = \"a\\\"b\";", "}"});
    CppLexer lexer;

    const auto tokensByLine = lexer.tokenize(buffer);

    ASSERT_EQ(tokensByLine.size(), 4u);

    ASSERT_EQ(tokensByLine[0].size(), 5u);
    expectToken(tokensByLine[0][0], 0u, 2u, TokenType::Keyword);
    expectToken(tokensByLine[0][1], 3u, 1u, TokenType::OpenParen);
    expectToken(tokensByLine[0][2], 4u, 1u, TokenType::Normal);
    expectToken(tokensByLine[0][3], 5u, 1u, TokenType::CloseParen);
    expectToken(tokensByLine[0][4], 7u, 1u, TokenType::OpenCurly);

    ASSERT_EQ(tokensByLine[1].size(), 5u);
    expectToken(tokensByLine[1][0], 0u, 4u, TokenType::Keyword);
    expectToken(tokensByLine[1][1], 5u, 1u, TokenType::Normal);
    expectToken(tokensByLine[1][2], 7u, 1u, TokenType::Unknown);
    expectToken(tokensByLine[1][3], 9u, 4u, TokenType::String);
    expectToken(tokensByLine[1][4], 13u, 1u, TokenType::Unknown);

    ASSERT_EQ(tokensByLine[2].size(), 8u);
    expectToken(tokensByLine[2][0], 0u, 3u, TokenType::Normal);
    expectToken(tokensByLine[2][1], 3u, 1u, TokenType::Unknown);
    expectToken(tokensByLine[2][2], 4u, 1u, TokenType::Unknown);
    expectToken(tokensByLine[2][3], 5u, 6u, TokenType::Normal);
    expectToken(tokensByLine[2][4], 12u, 1u, TokenType::Normal);
    expectToken(tokensByLine[2][5], 14u, 1u, TokenType::Unknown);
    expectToken(tokensByLine[2][6], 16u, 6u, TokenType::String);
    expectToken(tokensByLine[2][7], 22u, 1u, TokenType::Unknown);

    ASSERT_EQ(tokensByLine[3].size(), 1u);
    expectToken(tokensByLine[3][0], 0u, 1u, TokenType::CloseCurly);
}

TEST(SyntaxHighlighterTests, CppLanguageUsesCppLexer)
{
    TextBuffer buffer(std::vector<std::string>{"return value;"});
    SyntaxHighlighter highlighter;

    const auto tokensByLine = highlighter.tokenize(buffer, Language::Cpp);

    ASSERT_EQ(tokensByLine.size(), 1u);
    ASSERT_EQ(tokensByLine[0].size(), 3u);
    expectToken(tokensByLine[0][0], 0u, 6u, TokenType::Keyword);
    expectToken(tokensByLine[0][1], 7u, 5u, TokenType::Normal);
    expectToken(tokensByLine[0][2], 12u, 1u, TokenType::Unknown);
}

TEST(SyntaxHighlighterTests, UnsupportedLanguageReturnsEmptyTokenSet)
{
    TextBuffer buffer(std::vector<std::string>{"int x = 1;"});
    SyntaxHighlighter highlighter;

    const auto tokensByLine = highlighter.tokenize(buffer, Language::PlainText);

    EXPECT_TRUE(tokensByLine.empty());
}
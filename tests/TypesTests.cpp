#include <gtest/gtest.h>

#include "types.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

TEST(Types, PositionEqualityAndInequality)
{
    const Position p1{.row = 3, .col = 7};
    const Position p2{.row = 3, .col = 7};
    const Position p3{.row = 3, .col = 8};

    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 != p2);
    EXPECT_TRUE(p1 != p3);
}

TEST(Types, PositionOrderingUsesRowThenColumn)
{
    const Position a{.row = 1, .col = 9};
    const Position b{.row = 2, .col = 0};
    const Position c{.row = 2, .col = 1};

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(c > b);
    EXPECT_FALSE(b < a);
}

TEST(Types, PositionStreamFormatting)
{
    const Position pos{.row = 10, .col = 22};
    std::ostringstream stream;

    stream << pos;

    EXPECT_EQ(stream.str(), "(10, 22)");
}

TEST(Types, CursorCanAssignFromPosition)
{
    Cursor cursor;
    const Position pos{.row = 4, .col = 5};

    cursor = pos;

    EXPECT_EQ(cursor.row, 4u);
    EXPECT_EQ(cursor.col, 5u);
}

TEST(Types, SelectionEmptyWhenBeginEqualsEnd)
{
    const Selection selection{
        .begin = Position{.row = 2, .col = 1},
        .end = Position{.row = 2, .col = 1},
    };

    EXPECT_TRUE(selection.empty());
}

TEST(Types, SelectionNormalizedPreservesOrderedRange)
{
    const Selection selection{
        .begin = Position{.row = 1, .col = 2},
        .end = Position{.row = 3, .col = 4},
    };

    const Selection normalized = selection.normalized();

    EXPECT_EQ(normalized.begin, selection.begin);
    EXPECT_EQ(normalized.end, selection.end);
}

TEST(Types, SelectionNormalizedSwapsReversedRange)
{
    const Selection selection{
        .begin = Position{.row = 5, .col = 8},
        .end = Position{.row = 2, .col = 3},
    };

    const Selection normalized = selection.normalized();

    EXPECT_EQ(normalized.begin, selection.end);
    EXPECT_EQ(normalized.end, selection.begin);
}

TEST(Types, TokenTypeStreamFormatting)
{
    const std::vector<std::pair<TokenType, std::string>> expectations = {
        {TokenType::Normal, "Normal"},
        {TokenType::OpenParen, "OpenParen"},
        {TokenType::CloseParen, "CloseParen"},
        {TokenType::OpenCurly, "OpenCurly"},
        {TokenType::CloseCurly, "CloseCurly"},
        {TokenType::Keyword, "Keyword"},
        {TokenType::String, "String"},
        {TokenType::Number, "Number"},
        {TokenType::Comment, "Comment"},
        {TokenType::Preprocessor, "Preprocessor"},
        {TokenType::IncludeLib, "IncludeLib"},
        {TokenType::Unknown, "Unknown"},
    };

    for (const auto &[type, expectedText] : expectations)
    {
        std::ostringstream stream;
        stream << type;
        EXPECT_EQ(stream.str(), expectedText);
    }
}

TEST(Types, RectConstructorsSetValues)
{
    const Rect defaultRect;
    const Rect customRect(1, 2, 3, 4);

    EXPECT_EQ(defaultRect.x, 0u);
    EXPECT_EQ(defaultRect.y, 0u);
    EXPECT_EQ(defaultRect.w, 0u);
    EXPECT_EQ(defaultRect.h, 0u);

    EXPECT_EQ(customRect.x, 1u);
    EXPECT_EQ(customRect.y, 2u);
    EXPECT_EQ(customRect.w, 3u);
    EXPECT_EQ(customRect.h, 4u);
}

#include <gtest/gtest.h>
#include "TextBuffer.h"

TEST(TextBuffer, InsertFormattedSingleLine)
{
    TextBuffer buf(std::vector<std::string>{""});
    Position p = buf.insertFormatted(0, 0, "hello");
    EXPECT_EQ(buf.getLineCount(), 1u);
    EXPECT_EQ(buf.getLine(0), "hello");
    EXPECT_EQ(p.row, 0u);
    EXPECT_EQ(p.col, 5u);
}

TEST(TextBuffer, InsertFormattedMultiLine)
{
    TextBuffer buf(std::vector<std::string>{"abc"});
    Position p = buf.insertFormatted(0, 1, "X\nY");
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "aX");
    EXPECT_EQ(buf.getLine(1), "Ybc");
    EXPECT_EQ(p.row, 1u);
    EXPECT_EQ(p.col, 1u);
}

TEST(TextBuffer, InsertFormattedClampsOutOfRangeColumn)
{
    TextBuffer buf(std::vector<std::string>{"abc"});
    Position p = buf.insertFormatted(0, 99, "X");
    EXPECT_EQ(buf.getLine(0), "abcX");
    EXPECT_EQ(p.row, 0u);
    EXPECT_EQ(p.col, 4u);
}

TEST(TextBuffer, InsertLineOutOfRangeAppends)
{
    TextBuffer buf(std::vector<std::string>{"a"});
    buf.insertLine(99, "b");
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "a");
    EXPECT_EQ(buf.getLine(1), "b");
}

TEST(TextBuffer, EraseRangeSmartSingleLine)
{
    TextBuffer buf(std::vector<std::string>{"hello world"});
    buf.eraseRangeSmart(Position{0, 6}, 5);
    EXPECT_EQ(buf.getLineCount(), 1u);
    EXPECT_EQ(buf.getLine(0), "hello ");
}

TEST(TextBuffer, EraseRangeSmartAcrossNewline)
{
    TextBuffer buf(std::vector<std::string>{"abc", "def", "ghi"});
    buf.eraseRangeSmart(Position{0, 3}, 1);
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "abcdef");
}

TEST(TextBuffer, GetTextSliceMultiLine)
{
    TextBuffer buf(std::vector<std::string>{"abc", "def", "ghi"});
    Position s{0, 1};
    Position e{2, 2};
    std::string slice = buf.getTextSlice(s, e);
    EXPECT_EQ(slice, "bc\ndef\ngh");
}

TEST(TextBuffer, GetTextSliceInvalidRowsReturnsEmpty)
{
    TextBuffer buf(std::vector<std::string>{"abc"});
    Position s{2, 0};
    Position e{2, 1};
    EXPECT_EQ(buf.getTextSlice(s, e), "");
}

TEST(TextBuffer, GetTextSliceClampsColumns)
{
    TextBuffer buf(std::vector<std::string>{"abc", "def"});
    Position s{0, 99};
    Position e{1, 99};
    EXPECT_EQ(buf.getTextSlice(s, e), "\ndef");
}

TEST(TextBuffer, EraseRangeSmartStartColumnOutOfRangeIsSafe)
{
    TextBuffer buf(std::vector<std::string>{"abc"});
    buf.eraseRangeSmart(Position{0, 99}, 5);
    EXPECT_EQ(buf.getLineCount(), 1u);
    EXPECT_EQ(buf.getLine(0), "abc");
}

TEST(TextBuffer, EraseRangeInvalidInputNoOp)
{
    TextBuffer buf(std::vector<std::string>{"abc"});
    buf.eraseRange(0, 10, 11);
    EXPECT_EQ(buf.getLine(0), "abc");
}

TEST(TextBuffer, EraseRangeMultiRowInvalidColumnsNoOp)
{
    TextBuffer buf(std::vector<std::string>{"abc", "def"});
    buf.eraseRangeMultiRow(0, 10, 1, 1);
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "abc");
    EXPECT_EQ(buf.getLine(1), "def");
}

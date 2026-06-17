#include <gtest/gtest.h>
#include "../src/TextBuffer.h"

TEST(TextBuffer, InsertFormattedSingleLine) {
    TextBuffer buf(std::vector<std::string>{""});
    Position p = buf.insertFormatted(0, 0, "hello");
    EXPECT_EQ(buf.getLineCount(), 1u);
    EXPECT_EQ(buf.getLine(0), "hello");
    EXPECT_EQ(p.row, 0u);
    EXPECT_EQ(p.col, 5u);
}

TEST(TextBuffer, InsertFormattedMultiLine) {
    TextBuffer buf(std::vector<std::string>{"abc"});
    Position p = buf.insertFormatted(0, 1, "X\nY");
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "aX");
    EXPECT_EQ(buf.getLine(1), "Ybc");
    EXPECT_EQ(p.row, 1u);
    EXPECT_EQ(p.col, 1u);
}

TEST(TextBuffer, SplitLineAndMergeWithNext) {
    TextBuffer buf(std::vector<std::string>{"abcd"});
    buf.splitLine(0, 2);
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "ab");
    EXPECT_EQ(buf.getLine(1), "cd");
    buf.mergeWithNext(0);
    EXPECT_EQ(buf.getLineCount(), 1u);
    EXPECT_EQ(buf.getLine(0), "abcd");
}

TEST(TextBuffer, EraseRangeSmartSingleLine) {
    TextBuffer buf(std::vector<std::string>{"hello world"});
    buf.eraseRangeSmart(Position{0, 6}, 5);
    EXPECT_EQ(buf.getLineCount(), 1u);
    EXPECT_EQ(buf.getLine(0), "hello ");
}

TEST(TextBuffer, EraseRangeSmartAcrossNewline) {
    TextBuffer buf(std::vector<std::string>{"abc", "def", "ghi"});
    buf.eraseRangeSmart(Position{0, 3}, 1);
    EXPECT_EQ(buf.getLineCount(), 2u);
    EXPECT_EQ(buf.getLine(0), "abcdef");
}

TEST(TextBuffer, GetTextSliceMultiLine) {
    TextBuffer buf(std::vector<std::string>{"abc", "def", "ghi"});
    Position s{0, 1};
    Position e{2, 2};
    std::string slice = buf.getTextSlice(s, e);
    EXPECT_EQ(slice, "bc\ndef\ngh");
}

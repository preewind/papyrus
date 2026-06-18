#include <gtest/gtest.h>
#include "../src/TextBuffer.h"
#include "../src/UndoManager.h"
#include "../src/types.h"

TEST(UndoManager, SingleLineDeleteUndoRedo)
{
    TextBuffer buffer(std::vector<std::string>{"hello world"});
    DeleteAction action({0, 6}, "world");

    action.redo(buffer);
    EXPECT_EQ(buffer.getLineCount(), 1u);
    EXPECT_EQ(buffer.getLine(0), "hello ");

    Position undoCursor = action.undo(buffer);
    EXPECT_EQ(buffer.getLine(0), "hello world");
    EXPECT_EQ(undoCursor.row, 0u);
    EXPECT_EQ(undoCursor.col, 11u);
}

TEST(UndoManager, MultiLineDeleteUndoRedo)
{
    TextBuffer buffer(std::vector<std::string>{"abc", "def", "ghi"});
    Position start{0, 1};
    Position end{2, 2};
    std::string deletedText = buffer.getTextSlice(start, end);
    DeleteAction action(start, deletedText);

    action.redo(buffer);
    EXPECT_EQ(buffer.getLineCount(), 1u);
    EXPECT_EQ(buffer.getLine(0), "ai");

    Position undoCursor = action.undo(buffer);
    EXPECT_EQ(buffer.getLineCount(), 3u);
    EXPECT_EQ(buffer.getLine(0), "abc");
    EXPECT_EQ(buffer.getLine(1), "def");
    EXPECT_EQ(buffer.getLine(2), "ghi");
    EXPECT_EQ(undoCursor.row, 2u);
    EXPECT_EQ(undoCursor.col, 2u);
}

TEST(UndoManager, ConsecutiveBackspaceDeletesMergeIntoSingleUndo)
{
    TextBuffer buffer(std::vector<std::string>{"abcde"});
    UndoManager manager;

    auto firstDelete = std::make_unique<DeleteAction>(Position{0, 4}, "e");
    firstDelete->redo(buffer);
    manager.push(std::move(firstDelete));

    auto secondDelete = std::make_unique<DeleteAction>(Position{0, 3}, "d");
    secondDelete->redo(buffer);
    manager.push(std::move(secondDelete));

    EXPECT_EQ(buffer.getLine(0), "abc");
    EXPECT_TRUE(manager.canUndo());

    Position undoCursor = manager.undo(buffer);
    EXPECT_EQ(buffer.getLine(0), "abcde");
    EXPECT_EQ(undoCursor.row, 0u);
    EXPECT_EQ(undoCursor.col, 5u);
    EXPECT_FALSE(manager.canUndo());
}

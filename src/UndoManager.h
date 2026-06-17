#pragma once

#include <string>
#include <vector>

#include "types.h"
#include "TextBuffer.h"
#include <memory>

class EditAction
{

public:
    virtual ~EditAction() = default;

    virtual Position undo(TextBuffer &buffer) = 0;
    virtual Position redo(TextBuffer &buffer) = 0;
};

class InsertAction : public EditAction
{
public:
    Position position;
    std::string text;

    InsertAction(Position pos, std::string text);

    Position undo(TextBuffer &buffer) override;
    Position redo(TextBuffer &buffer) override;
};

class DeleteAction : public EditAction
{
public:
    Position position;
    std::string text;

    DeleteAction(Position pos, std::string text);

    Position undo(TextBuffer &buffer) override;
    Position redo(TextBuffer &buffer) override;
};

class UndoManager
{
public:
    void push(std::unique_ptr<EditAction> action);
    Position undo(TextBuffer &buffer);
    Position redo(TextBuffer &buffer);

    bool canUndo() const { return !mUndoStack.empty(); }
    bool canRedo() const { return !mRedoStack.empty(); }

private:
    std::vector<std::unique_ptr<EditAction>> mUndoStack;
    std::vector<std::unique_ptr<EditAction>> mRedoStack;
};
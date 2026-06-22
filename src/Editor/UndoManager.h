#pragma once

#include <string>
#include <vector>

#include "types.h"
#include "TextBuffer.h"
#include <memory>

/**
 * @brief Represents a single change made to the text buffer.
 * * Think of this as a recipe that knows how to apply a change (redo)
 * and how to completely reverse that exact same change (undo).
 */
class EditAction
{

public:
    virtual ~EditAction() = default;

    virtual Position undo(TextBuffer &buffer) = 0;
    virtual Position redo(TextBuffer &buffer) = 0;
    virtual bool tryMerge(const EditAction &action)
    {
        (void)action;
        return false;
    }
};

/**
 * @brief Handles text addition (typing, pasting, hitting enter).
 * * - Undo: Deletes the exact length of text that was inserted.
 * - Redo: Re-inserts the text back into the buffer.
 */
class InsertAction : public EditAction
{
public:
    Position position;
    std::string text;

    InsertAction(Position pos, std::string text);

    Position undo(TextBuffer &buffer) override;
    Position redo(TextBuffer &buffer) override;
    bool tryMerge(const EditAction &action) override;
};

/**
 * @brief Handles text removal (backspace, delete key, cutting text).
 * * Crucially, this stores the string that was deleted.
 * - Undo: Puts the deleted text back exactly where it used to be.
 * - Redo: Wipes the text out of the buffer again.
 */
class DeleteAction : public EditAction
{
public:
    Position position;
    std::string text;

    DeleteAction(Position pos, std::string text);

    Position undo(TextBuffer &buffer) override;
    Position redo(TextBuffer &buffer) override;
    bool tryMerge(const EditAction &action) override;
};

/**
 * @brief The brains of the history system. Tracks what can be undone and redone.
 * * It manages two stacks of actions.
 * - When you type normally: Actions go to the Undo stack, and the Redo stack is cleared.
 * - When you Undo: An action pops off the Undo stack, runs its undo() code, and moves to Redo.
 * - When you Redo: An action pops off the Redo stack, runs its redo() code, and moves to Undo.
 */
class UndoManager
{
public:
    void push(std::unique_ptr<EditAction> action);
    Position undo(TextBuffer &buffer);
    Position redo(TextBuffer &buffer);
    void clear();

    bool canUndo() const { return !mUndoStack.empty(); }
    bool canRedo() const { return !mRedoStack.empty(); }

private:
    std::vector<std::unique_ptr<EditAction>> mUndoStack;
    std::vector<std::unique_ptr<EditAction>> mRedoStack;
};
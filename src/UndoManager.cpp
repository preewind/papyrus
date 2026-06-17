#include "UndoManager.h"

InsertAction::InsertAction(Position pos, std::string text) : position(pos), text(std::move(text))
{
}

Position InsertAction::undo(TextBuffer &buffer)
{
    buffer.eraseRange(position, text.size());
    return position;
}

Position InsertAction::redo(TextBuffer &buffer)
{
    buffer.insertFormatted(position.row, position.col, text);
    return {position.row, position.col + text.size()};
}

DeleteAction::DeleteAction(Position pos, std::string text) : position(pos), text(std::move(text))
{
}

Position DeleteAction::undo(TextBuffer &buffer)
{
    buffer.insertFormatted(position.row, position.col, text);
    return {position.row, position.col + text.size()};
}

Position DeleteAction::redo(TextBuffer &buffer)
{
    buffer.eraseRange(position, text.size());
    return position;
}

void UndoManager::push(std::unique_ptr<EditAction> action)
{
    mUndoStack.push_back(std::move(action));
    mRedoStack.clear();
}

Position UndoManager::undo(TextBuffer &buffer)
{
    if (!canUndo())
    {
        return {0, 0};
    }

    std::unique_ptr<EditAction> action = std::move(mUndoStack.back());
    mUndoStack.pop_back();
    Position newCursorPos = action->undo(buffer);
    mRedoStack.push_back(std::move(action));
    return newCursorPos;
}

Position UndoManager::redo(TextBuffer &buffer)
{
    if (!canRedo())
    {
        return {0, 0};
    }

    std::unique_ptr<EditAction> action = std::move(mRedoStack.back());
    mRedoStack.pop_back();
    Position newCursorPos = action->redo(buffer);
    mUndoStack.push_back(std::move(action));
    return newCursorPos;
}

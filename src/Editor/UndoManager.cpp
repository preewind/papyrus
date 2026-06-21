#include <cctype>

#include "UndoManager.h"

InsertAction::InsertAction(Position pos, std::string text) : position(pos), text(std::move(text))
{
}

Position InsertAction::undo(TextBuffer &buffer)
{
    buffer.eraseRangeSmart(position, text.size());
    return position;
}

Position InsertAction::redo(TextBuffer &buffer)
{
    return buffer.insertFormatted(position.row, position.col, text);
}

/**
 * @brief Tries to merge a newly typed character into this action.
 * * Instead of creating a brand new undo step for every single keystroke,
 * this groups consecutive characters together. It stops merging and starts
 * a new group whenever the user types a space, tab, or newline.
 */
bool InsertAction::tryMerge(const EditAction &action)
{
    // try to cast nextAction to an InsertAction
    auto *next = dynamic_cast<const InsertAction *>(&action);
    if (!next)
    {
        return false;
    }
    if (next->position.row != this->position.row ||
        next->position.col != this->position.col + (int)this->text.size())
    {
        return false;
    }
    if (!this->text.empty() && !next->text.empty())
    {
        char lastChar = this->text.back();
        char nextChar = next->text.front();

        if (std::isspace(static_cast<unsigned char>(lastChar)) ||
            std::isspace(static_cast<unsigned char>(nextChar)))
        {
            return false; // Break the merge! This creates a new undo group.
        }
    }
    this->text += next->text;
    return true;
}

DeleteAction::DeleteAction(Position pos, std::string text): position(pos), text(std::move(text)) {}

bool DeleteAction::tryMerge(const EditAction &action)
{
    auto *next = dynamic_cast<const DeleteAction *>(&action);
    if (!next)
    {
        return false;
    }

    // Sequential delete-key actions delete from the same position.
    if (next->position == this->position)
    {
        this->text += next->text;
        return true;
    }

    // Sequential backspace actions delete immediately before the previous delete.
    if (next->position.row == this->position.row &&
        next->position.col + next->text.size() == this->position.col)
    {
        this->position = next->position;
        this->text = next->text + this->text;
        return true;
    }

    return false;
}

Position DeleteAction::undo(TextBuffer &buffer)
{
    return buffer.insertFormatted(position.row, position.col, text);
}

Position DeleteAction::redo(TextBuffer &buffer)
{
    buffer.eraseRangeSmart(position, text.size());
    return position;
}

void UndoManager::push(std::unique_ptr<EditAction> action)
{
    mRedoStack.clear();
    if (!mUndoStack.empty() && mUndoStack.back()->tryMerge(*action))
    {
        return;
    }

    mUndoStack.push_back(std::move(action));
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

#include "TextInput.h"

#include <algorithm>

#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_keycode.h>

bool TextInput::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_TEXT_INPUT)
    {
        insert(event.text.text);
        return true;
    }

    if (event.type != SDL_EVENT_KEY_DOWN)
    {
        return false;
    }

    bool shiftHeld = event.key.mod & SDL_KMOD_SHIFT;
    bool ctrlHeld = event.key.mod & SDL_KMOD_CTRL;

    switch (event.key.key)
    {
    case SDLK_BACKSPACE:
        backspace();
        return true;
    case SDLK_DELETE:
        del();
        return true;
    case SDLK_LEFT:
        if (shiftHeld)
        {
            beginSelection();
        }
        else
        {
            clearSelection();
        }
        moveLeft();
        if (shiftHeld)
        {
            updateSelection();
        }
        return true;
    case SDLK_RIGHT:
        if (shiftHeld)
        {
            beginSelection();
        }
        else
        {
            clearSelection();
        }
        moveRight();
        if (shiftHeld)
        {
            updateSelection();
        }
        return true;
    case SDLK_HOME:
        if (shiftHeld)
        {
            beginSelection();
        }
        else
        {
            clearSelection();
        }
        moveHome();
        if (shiftHeld)
        {
            updateSelection();
        }
        return true;
    case SDLK_END:
        if (shiftHeld)
        {
            beginSelection();
        }
        else
        {
            clearSelection();
        }
        moveEnd();
        if (shiftHeld)
        {
            updateSelection();
        }
        return true;
    case SDLK_Z:
        if (ctrlHeld)
        {
            undo();
            return true;
        }
        return false;
    case SDLK_Y:
        if (ctrlHeld)
        {
            redo();
            return true;
        }
        return false;
    case SDLK_C:
        if (ctrlHeld)
        {
            copySelectionToClipboard();
            return true;
        }
        return false;
    case SDLK_X:
        if (ctrlHeld)
        {
            cutSelectionToClipboard();
            return true;
        }
        return false;
    case SDLK_V:
        if (ctrlHeld)
        {
            pasteClipboardText();
            return true;
        }
        return false;
    default:
        return false;
    }
}

void TextInput::insert(const std::string &text)
{
    if (text.empty())
    {
        return;
    }

    saveSnapshot();

    if (hasSelection())
    {
        TextSelection normalized = mSelection.normalized();
        mText.erase(normalized.begin, normalized.end - normalized.begin);
        mCursor = normalized.begin;
        clearSelection();
    }

    mText.insert(mCursor, text);
    mCursor += text.size();
}

void TextInput::backspace()
{
    if (hasSelection())
    {
        saveSnapshot();
        TextSelection normalized = mSelection.normalized();
        mText.erase(normalized.begin, normalized.end - normalized.begin);
        mCursor = normalized.begin;
        clearSelection();
        return;
    }

    if (mCursor == 0)
    {
        return;
    }

    saveSnapshot();
    mText.erase(mCursor - 1, 1);
    mCursor--;
}

void TextInput::del()
{
    if (hasSelection())
    {
        saveSnapshot();
        TextSelection normalized = mSelection.normalized();
        mText.erase(normalized.begin, normalized.end - normalized.begin);
        mCursor = normalized.begin;
        clearSelection();
        return;
    }

    if (mCursor >= mText.size())
    {
        return;
    }

    saveSnapshot();
    mText.erase(mCursor, 1);
}

void TextInput::moveLeft()
{
    if (mCursor > 0)
    {
        mCursor--;
    }
}

void TextInput::moveRight()
{
    if (mCursor < mText.size())
    {
        mCursor++;
    }
}

void TextInput::moveHome()
{
    mCursor = 0;
}

void TextInput::moveEnd()
{
    mCursor = mText.size();
}

void TextInput::setCursor(uint32_t cursor)
{
    mCursor = std::min<uint32_t>(cursor, mText.size());
}

void TextInput::addToCursor(uint32_t amount)
{
    setCursor(mCursor + amount);
}

void TextInput::clear()
{
    mText.clear();
    mCursor = 0;
    clearSelection();
    mUndoStack.clear();
    mRedoStack.clear();
}

const std::string &TextInput::getText() const
{
    return mText;
}

uint32_t TextInput::getCursor() const
{
    return mCursor;
}

void TextInput::beginSelection()
{
    if (mSelectionActive)
    {
        return;
    }
    mSelectionActive = true;
    mSelection.begin = mCursor;
    mSelection.end = mCursor;
}

void TextInput::updateSelection()
{
    if (!mSelectionActive)
    {
        return;
    }
    mSelection.end = mCursor;
}

void TextInput::clearSelection()
{
    mSelectionActive = false;
    mSelection.begin = mCursor;
    mSelection.end = mCursor;
}

bool TextInput::hasSelection() const
{
    return mSelectionActive && !mSelection.empty();
}

TextSelection TextInput::getSelection() const
{
    return mSelection;
}

std::string TextInput::selectedText() const
{
    if (!hasSelection())
    {
        return std::string();
    }

    TextSelection normalized = mSelection.normalized();
    return mText.substr(normalized.begin, normalized.end - normalized.begin);
}

void TextInput::copySelectionToClipboard()
{
    if (!hasSelection())
    {
        return;
    }

    SDL_SetClipboardText(selectedText().c_str());
}

void TextInput::cutSelectionToClipboard()
{
    if (!hasSelection())
    {
        return;
    }

    SDL_SetClipboardText(selectedText().c_str());
    del();
}

void TextInput::pasteClipboardText()
{
    std::string text = SDL_GetClipboardText();
    if (!text.empty())
    {
        insert(text);
    }
}

void TextInput::saveSnapshot()
{
    mUndoStack.push_back({mText, mCursor});
    mRedoStack.clear();
}

void TextInput::undo()
{
    if (mUndoStack.empty())
    {
        return;
    }
    mRedoStack.push_back({mText, mCursor});
    Snapshot snapshot = mUndoStack.back();
    mUndoStack.pop_back();
    mText = snapshot.text;
    mCursor = snapshot.cursor;
    clearSelection();
}

void TextInput::redo()
{
    if (mRedoStack.empty())
    {
        return;
    }
    mUndoStack.push_back({mText, mCursor});
    Snapshot snapshot = mRedoStack.back();
    mRedoStack.pop_back();
    mText = snapshot.text;
    mCursor = snapshot.cursor;
    clearSelection();
}

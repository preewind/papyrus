#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include <SDL3/SDL_events.h>

struct TextSelection
{
    uint32_t begin = 0;
    uint32_t end = 0;

    bool empty() const
    {
        return begin == end;
    }

    TextSelection normalized() const
    {
        if (begin <= end)
        {
            return *this;
        }
        return {end, begin};
    }
};

class TextInput
{
public:
    bool handleKey(const SDL_Event &event);

    void insert(const std::string &text);
    void backspace();
    void del();
    void moveLeft();
    void moveRight();
    void moveHome();
    void moveEnd();
    void setCursor(uint32_t cursor);
    void addToCursor(uint32_t amount);
    void clear();
    void setText(const std::string &text);

    const std::string &getText() const;
    uint32_t getCursor() const;

    void beginSelection();
    void updateSelection();
    void clearSelection();
    bool hasSelection() const;
    TextSelection getSelection() const;
    std::string getSelectedText() const;

    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;

private:
    struct Snapshot
    {
        std::string text;
        uint32_t cursor;
    };

    void saveSnapshot();

    std::string mText;
    uint32_t mCursor = 0;
    bool mSelectionActive = false;
    TextSelection mSelection;
    std::vector<Snapshot> mUndoStack;
    std::vector<Snapshot> mRedoStack;
};

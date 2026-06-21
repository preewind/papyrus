#include <gtest/gtest.h>

#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_events.h>

#include "TextInput.h"

static SDL_Event makeKeyDown(SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE)
{
    SDL_Event e{};
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.key = key;
    e.key.mod = mod;
    return e;
}

static SDL_Event makeTextEvent(const char *text)
{
    SDL_Event e{};
    e.type = SDL_EVENT_TEXT_INPUT;
    e.text.text = text;
    return e;
}

TEST(TextInput, InsertAndCursorMovesForward)
{
    TextInput input;
    input.insert("abc");

    EXPECT_EQ(input.getText(), "abc");
    EXPECT_EQ(input.getCursor(), 3u);
}

TEST(TextInput, BackspaceRemovesCharacterBeforeCursor)
{
    TextInput input;
    input.insert("abc");
    input.backspace();

    EXPECT_EQ(input.getText(), "ab");
    EXPECT_EQ(input.getCursor(), 2u);
}

TEST(TextInput, DeleteRemovesCharacterAtCursor)
{
    TextInput input;
    input.insert("abc");
    input.moveHome();
    input.del();

    EXPECT_EQ(input.getText(), "bc");
    EXPECT_EQ(input.getCursor(), 0u);
}

TEST(TextInput, HandleKeyProcessesTextAndNavigation)
{
    TextInput input;
    EXPECT_TRUE(input.handleKey(makeTextEvent("hello")));
    EXPECT_TRUE(input.handleKey(makeKeyDown(SDLK_LEFT)));
    EXPECT_TRUE(input.handleKey(makeKeyDown(SDLK_BACKSPACE)));

    EXPECT_EQ(input.getText(), "helo");
    EXPECT_EQ(input.getCursor(), 3u);
}

TEST(TextInput, SelectionReplaceAndDelete)
{
    TextInput input;
    input.insert("abcdef");

    input.setCursor(1);
    input.beginSelection();
    input.setCursor(4);
    input.updateSelection();

    input.insert("X");
    EXPECT_EQ(input.getText(), "aXef");
    EXPECT_EQ(input.getCursor(), 2u);

    input.beginSelection();
    input.setCursor(4);
    input.updateSelection();
    input.del();

    EXPECT_EQ(input.getText(), "aX");
    EXPECT_EQ(input.getCursor(), 2u);
}

TEST(TextInput, ShiftLeftCreatesSelection)
{
    TextInput input;
    input.insert("hello");

    input.handleKey(makeKeyDown(SDLK_LEFT, SDL_KMOD_SHIFT));
    input.handleKey(makeKeyDown(SDLK_LEFT, SDL_KMOD_SHIFT));

    EXPECT_TRUE(input.hasSelection());
    TextSelection sel = input.getSelection().normalized();
    EXPECT_EQ(sel.begin, 3u);
    EXPECT_EQ(sel.end, 5u);
}

TEST(TextInput, ShiftRightShrinksSelectionCreatedByShiftLeft)
{
    TextInput input;
    input.insert("hello");

    input.handleKey(makeKeyDown(SDLK_LEFT, SDL_KMOD_SHIFT));
    input.handleKey(makeKeyDown(SDLK_LEFT, SDL_KMOD_SHIFT));
    input.handleKey(makeKeyDown(SDLK_RIGHT, SDL_KMOD_SHIFT));

    EXPECT_TRUE(input.hasSelection());
    TextSelection sel = input.getSelection().normalized();
    EXPECT_EQ(sel.begin, 4u);
    EXPECT_EQ(sel.end, 5u);
}

TEST(TextInput, PlainArrowClearsSelection)
{
    TextInput input;
    input.insert("hello");

    input.handleKey(makeKeyDown(SDLK_LEFT, SDL_KMOD_SHIFT));
    EXPECT_TRUE(input.hasSelection());

    input.handleKey(makeKeyDown(SDLK_LEFT));
    EXPECT_FALSE(input.hasSelection());
}

TEST(TextInput, UndoRevertsLastInsert)
{
    TextInput input;
    input.insert("hello");

    input.handleKey(makeKeyDown(SDLK_Z, SDL_KMOD_CTRL));

    EXPECT_EQ(input.getText(), "");
}

TEST(TextInput, RedoReappliesInsert)
{
    TextInput input;
    input.insert("hello");
    input.handleKey(makeKeyDown(SDLK_Z, SDL_KMOD_CTRL));

    input.handleKey(makeKeyDown(SDLK_Y, SDL_KMOD_CTRL));

    EXPECT_EQ(input.getText(), "hello");
    EXPECT_EQ(input.getCursor(), 5u);
}

TEST(TextInput, NewMutationClearsRedoStack)
{
    TextInput input;
    input.insert("hello");
    input.undo();

    input.insert("world");
    input.redo();
    EXPECT_EQ(input.getText(), "world");
}

TEST(TextInput, UndoBackspaceRestoresText)
{
    TextInput input;
    input.insert("abc");
    input.backspace();
    EXPECT_EQ(input.getText(), "ab");

    input.undo();
    EXPECT_EQ(input.getText(), "abc");
    EXPECT_EQ(input.getCursor(), 3u);
}

TEST(TextInput, ClearResetsUndoStack)
{
    TextInput input;
    input.insert("hello");
    input.clear();

    EXPECT_EQ(input.getText(), "");
    input.undo();
    EXPECT_EQ(input.getText(), "");
}

TEST(TextInput, CtrlCCopiesSelectedText)
{
    TextInput input;
    input.insert("abcdef");

    input.setCursor(1);
    input.beginSelection();
    input.setCursor(4);
    input.updateSelection();

    if (!SDL_SetClipboardText(""))
    {
        GTEST_SKIP() << "Clipboard not available in this environment";
    }

    EXPECT_TRUE(input.handleKey(makeKeyDown(SDLK_C, SDL_KMOD_CTRL)));
    EXPECT_EQ(std::string(SDL_GetClipboardText()), "bcd");
}

TEST(TextInput, CtrlXCutsSelectionAndCopiesText)
{
    TextInput input;
    input.insert("abcdef");

    input.setCursor(1);
    input.beginSelection();
    input.setCursor(4);
    input.updateSelection();

    if (!SDL_SetClipboardText(""))
    {
        GTEST_SKIP() << "Clipboard not available in this environment";
    }

    EXPECT_TRUE(input.handleKey(makeKeyDown(SDLK_X, SDL_KMOD_CTRL)));
    EXPECT_EQ(std::string(SDL_GetClipboardText()), "bcd");
    EXPECT_EQ(input.getText(), "aef");
    EXPECT_EQ(input.getCursor(), 1u);
}

TEST(TextInput, CtrlVPastesClipboardTextAtCursor)
{
    TextInput input;
    input.insert("ab");
    input.setCursor(1);

    if (!SDL_SetClipboardText("XYZ"))
    {
        GTEST_SKIP() << "Clipboard not available in this environment";
    }

    EXPECT_TRUE(input.handleKey(makeKeyDown(SDLK_V, SDL_KMOD_CTRL)));
    EXPECT_EQ(input.getText(), "aXYZb");
    EXPECT_EQ(input.getCursor(), 4u);
}

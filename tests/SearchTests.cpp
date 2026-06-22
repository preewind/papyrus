#include <gtest/gtest.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

#include "SearchEngine.h"
#include "SearchSession.h"
#include "TextBuffer.h"

namespace
{
SDL_Event makeKeyDown(SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE)
{
    SDL_Event e{};
    e.type = SDL_EVENT_KEY_DOWN;
    e.key.key = key;
    e.key.mod = mod;
    return e;
}

SDL_Event makeTextEvent(const char *text)
{
    SDL_Event e{};
    e.type = SDL_EVENT_TEXT_INPUT;
    e.text.text = text;
    return e;
}
} // namespace

TEST(SearchEngineTests, EmptyQueryReturnsNoMatches)
{
    TextBuffer buffer(std::vector<std::string>{"hello", "world"});
    SearchEngine engine;

    const auto matches = engine.find(buffer, "");

    EXPECT_TRUE(matches.empty());
}

TEST(SearchEngineTests, FindsMatchesAcrossMultipleLines)
{
    TextBuffer buffer(std::vector<std::string>{"alpha beta", "beta", "gamma beta"});
    SearchEngine engine;

    const auto matches = engine.find(buffer, "beta");

    ASSERT_EQ(matches.size(), 3u);
    EXPECT_EQ(matches[0].row, 0u);
    EXPECT_EQ(matches[0].col, 6u);
    EXPECT_EQ(matches[0].length, 4u);
    EXPECT_EQ(matches[1].row, 1u);
    EXPECT_EQ(matches[1].col, 0u);
    EXPECT_EQ(matches[1].length, 4u);
    EXPECT_EQ(matches[2].row, 2u);
    EXPECT_EQ(matches[2].col, 6u);
    EXPECT_EQ(matches[2].length, 4u);
}

TEST(SearchEngineTests, FindsOverlappingMatches)
{
    TextBuffer buffer(std::vector<std::string>{"aaaa"});
    SearchEngine engine;

    const auto matches = engine.find(buffer, "aa");

    ASSERT_EQ(matches.size(), 3u);
    EXPECT_EQ(matches[0].col, 0u);
    EXPECT_EQ(matches[1].col, 1u);
    EXPECT_EQ(matches[2].col, 2u);
}

TEST(SearchSessionTests, TextInputEventUpdatesQueryAndCursor)
{
    SearchSession session;
    Cursor editorCursor{4, 7};

    const Cursor returned = session.handleKey(makeTextEvent("needle"), editorCursor);

    EXPECT_EQ(returned.row, editorCursor.row);
    EXPECT_EQ(returned.col, editorCursor.col);
    EXPECT_EQ(session.getQuery(), "needle");
    EXPECT_EQ(session.getCursor(), 6u);
}

TEST(SearchSessionTests, DownCyclesThroughMatchesAndWraps)
{
    SearchSession session;
    session.setMatches({SearchMatch{2, 3, 4}, SearchMatch{5, 1, 4}});

    Cursor cursor{0, 0};
    cursor = session.handleKey(makeKeyDown(SDLK_DOWN), cursor);
    EXPECT_EQ(cursor.row, 5u);
    EXPECT_EQ(cursor.col, 1u);
    EXPECT_EQ(session.getCurrentMatchIndex(), 1u);

    cursor = session.handleKey(makeKeyDown(SDLK_DOWN), cursor);
    EXPECT_EQ(cursor.row, 2u);
    EXPECT_EQ(cursor.col, 3u);
    EXPECT_EQ(session.getCurrentMatchIndex(), 0u);
}

TEST(SearchSessionTests, UpCyclesThroughMatchesAndWraps)
{
    SearchSession session;
    session.setMatches({SearchMatch{1, 1, 3}, SearchMatch{4, 2, 3}, SearchMatch{7, 0, 3}});

    Cursor cursor{9, 9};
    cursor = session.handleKey(makeKeyDown(SDLK_UP), cursor);
    EXPECT_EQ(cursor.row, 7u);
    EXPECT_EQ(cursor.col, 0u);
    EXPECT_EQ(session.getCurrentMatchIndex(), 2u);

    cursor = session.handleKey(makeKeyDown(SDLK_UP), cursor);
    EXPECT_EQ(cursor.row, 4u);
    EXPECT_EQ(cursor.col, 2u);
    EXPECT_EQ(session.getCurrentMatchIndex(), 1u);
}

TEST(SearchSessionTests, ArrowNavigationWithoutMatchesKeepsEditorCursor)
{
    SearchSession session;
    Cursor editorCursor{3, 8};

    const Cursor up = session.handleKey(makeKeyDown(SDLK_UP), editorCursor);
    const Cursor down = session.handleKey(makeKeyDown(SDLK_DOWN), editorCursor);

    EXPECT_EQ(up.row, editorCursor.row);
    EXPECT_EQ(up.col, editorCursor.col);
    EXPECT_EQ(down.row, editorCursor.row);
    EXPECT_EQ(down.col, editorCursor.col);
    EXPECT_FALSE(session.hasMatches());
    EXPECT_EQ(session.getCurrentMatchIndex(), 0u);
}

TEST(SearchSessionTests, SetMatchesResetsCurrentMatchIndex)
{
    SearchSession session;
    session.setMatches({SearchMatch{0, 0, 1}, SearchMatch{0, 2, 1}});

    Cursor cursor{0, 0};
    cursor = session.handleKey(makeKeyDown(SDLK_DOWN), cursor);
    EXPECT_EQ(session.getCurrentMatchIndex(), 1u);

    session.setMatches({SearchMatch{9, 9, 2}});
    EXPECT_TRUE(session.hasMatches());
    EXPECT_EQ(session.getCurrentMatchIndex(), 0u);
    ASSERT_EQ(session.getMatches().size(), 1u);
    EXPECT_EQ(session.getMatches()[0].row, 9u);
    EXPECT_EQ(session.getMatches()[0].col, 9u);
}

TEST(SearchSessionTests, DelegatesSelectionStateToQueryInput)
{
    SearchSession session;
    Cursor editorCursor{0, 0};

    session.handleKey(makeTextEvent("abc"), editorCursor);
    session.handleKey(makeKeyDown(SDLK_LEFT, SDL_KMOD_SHIFT), editorCursor);

    EXPECT_TRUE(session.hasSelection());
    TextSelection selection = session.getSelection().normalized();
    EXPECT_EQ(selection.begin, 2u);
    EXPECT_EQ(selection.end, 3u);
}
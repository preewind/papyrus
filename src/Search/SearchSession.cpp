#include "SearchSession.h"

#include <SDL3/SDL_keycode.h>

Cursor SearchSession::handleKey(const SDL_Event &event, Cursor editorCursor)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        switch (event.key.key)
        {
        case SDLK_UP:
            return cycleUp(editorCursor);
        case SDLK_DOWN:
            return cycleDown(editorCursor);
        default:
            break;
        }
    }
    mQuery.handleKey(event);
    return editorCursor;
}

Cursor SearchSession::cycleUp(Cursor editorCursor)
{
    if (hasMatches())
    {
        mCurrentMatch = (mCurrentMatch + mMatches.size() - 1) % mMatches.size();
        SearchMatch match = mMatches[mCurrentMatch];
        return {match.row, match.col};
    }
    return editorCursor;
}

Cursor SearchSession::cycleDown(Cursor editorCursor)
{
    if (hasMatches())
    {
        mCurrentMatch = (mCurrentMatch + 1) % mMatches.size();
        SearchMatch match = mMatches[mCurrentMatch];
        return {match.row, match.col};
    }
    return editorCursor;
}

std::string SearchSession::getQuery() const
{
    return mQuery.getText();
}

uint32_t SearchSession::getCursor() const
{
    return mQuery.getCursor();
}

void SearchSession::setMatches(const std::vector<SearchMatch> &matches)
{
    mMatches = matches;
}

std::vector<SearchMatch> SearchSession::getMatches() const
{
    return mMatches;
}

uint32_t SearchSession::getCurrentMatchIndex() const
{
    return mCurrentMatch;
}

bool SearchSession::hasMatches() const
{
    return mMatches.size() > 0;
}

bool SearchSession::hasSelection() const
{
    return mQuery.hasSelection();
}

TextSelection SearchSession::getSelection() const
{
    return mQuery.getSelection();
}

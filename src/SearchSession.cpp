#include "SearchSession.h"

std::string SearchSession::getQuery() const
{
    return mQuery.getLine(0);
}

void SearchSession::addToQuery(const std::string &text)
{
    mQuery.insert(0, mCursor, text);
    mCursor += text.size();
}

void SearchSession::handleBackSpace()
{
    if (mCursor > 0)
    {
        mQuery.erase(0, mCursor - 1);
        mCursor--;
    }
}

void SearchSession::handleDelete()
{
    if (mCursor < mQuery.getLineSize(0))
    {
        mQuery.erase(0, mCursor);
    }
}

void SearchSession::handleLeft()
{
    if (mCursor > 0)
    {
        mCursor--;
    }
}

void SearchSession::handleRight()
{
    if (mCursor < mQuery.getLineSize(0))
    {
        mCursor++;
    }
}

void SearchSession::handleEnd()
{
    mCursor = mQuery.getLineSize(0);
}

Cursor SearchSession::handleUp(Cursor &cursor)
{
    if (hasMatches())
    {
        mCurrentMatch = (mCurrentMatch + mMatches.size() - 1) % mMatches.size();
        SearchMatch match = mMatches[mCurrentMatch];
        return {match.row, match.col};
    }
    return cursor;
}

Cursor SearchSession::handleDown(Cursor &cursor)
{
    if (hasMatches())
    {
        mCurrentMatch = (mCurrentMatch + 1) % mMatches.size();
        SearchMatch match = mMatches[mCurrentMatch];
        return {match.row, match.col};
    }
    return cursor;
}

void SearchSession::addToCursor(uint32_t size)
{
    mCursor += size;
}

void SearchSession::resetCursor()
{
    mCursor = 0;
}

uint32_t SearchSession::getCursor() const
{
    return mCursor;
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

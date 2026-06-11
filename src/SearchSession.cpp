#include "SearchSession.h"

std::string SearchSession::getQuery() const
{
    return mQuery;
}

void SearchSession::addToQuery(const std::string &text)
{
    mQuery += text;
}

void SearchSession::setMatches(const std::vector<SearchMatch> &matches)
{
    mMatches = matches;
}

std::vector<SearchMatch> SearchSession::getMatches() const
{
    return mMatches;
}

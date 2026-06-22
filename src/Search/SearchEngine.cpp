#include "SearchEngine.h"
#include "TextBuffer.h"
#include "logger.h"

std::vector<SearchMatch> SearchEngine::find(const TextBuffer &buffer, const std::string &query) const
{
    std::vector<SearchMatch> matches;
    if (query.empty())
    {
        return matches;
    }
    uint32_t lineCount = buffer.getLineCount();
    for (uint32_t i = 0; i < lineCount; i++)
    {
        const std::string &line = buffer.getLine(i);
        const uint32_t lineSize = line.size();
        if (lineSize < query.size())
        {
            continue;
        }
        for (uint32_t j = 0; j + query.size() <= lineSize; j++)
        {
            if (line[j] == query[0] && line.compare(j, query.size(), query) == 0)
            {
                matches.push_back(SearchMatch{i, j, (uint32_t)query.length()});
                LOG_DEBUG() << "Found match: " << line.substr(j, query.size()) << " Match: " << i << "," << j << ", " << query.length();
            }
        }
    }
    return matches;
}

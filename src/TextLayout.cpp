#include "TextLayout.h"

void TextLayout::setMeasurer(const ITextMeasurer *measurer)
{
    mMeasurer = measurer;
}

uint32_t TextLayout::width(const std::string &text) const
{
    if (text.empty())
    {
        return 0;
    }
    if (!mMeasurer)
    {
        return 0;
    }
    return mMeasurer->width(text);
}

std::string TextLayout::expandTabs(std::string_view text) const
{
    std::string result = "";

    for (char c : text)
    {
        if (c == '\t')
        {
            result += "    ";
        }
        else
        {
            result += c;
        }
    }
    return result;
}

uint32_t TextLayout::virtualColumn(std::string_view line, uint32_t rawCol) const
{
    uint32_t virtualCol = 0;
    for (uint32_t i = 0; i < rawCol && i < line.size(); ++i)
    {
        if (line[i] == '\t')
        {
            virtualCol += 4;
        }
        else
        {
            virtualCol += 1;
        }
    }
    return virtualCol;
}

int TextLayout::columnToPixel(std::string_view line, uint32_t col) const
{
    return width(expandTabs(line.substr(0, col)));
}

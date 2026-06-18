#include "TextLayout.h"
#include "util.h"

void TextLayout::setFont(TTF_Font *font)
{
    mFont = font;
}

uint32_t TextLayout::width(const std::string& text) const
{
    if (text.empty())
    {
        return 0;
    }
    int w = 0;
    int h = 0;

    CSF(TTF_GetStringSize(mFont, text.c_str(), 0, &w, &h));

    return w;
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

uint32_t TextLayout::virtualColumn(std::string_view line, uint32_t rawCol)
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

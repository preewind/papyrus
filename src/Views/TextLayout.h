#pragma once

#include <string>

#include "ITextMeasurer.h"

class TextLayout
{
public:
    TextLayout() = default;

    void setMeasurer(const ITextMeasurer *measurer);
    uint32_t width(const std::string &text) const;
    std::string expandTabs(std::string_view text) const;
    uint32_t virtualColumn(std::string_view line, uint32_t rawCol) const;
    int columnToPixel(std::string_view line, uint32_t col) const;

private:
    const ITextMeasurer *mMeasurer = nullptr;
};
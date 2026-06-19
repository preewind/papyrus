#pragma once

#include <cstdint>
#include <string_view>

class ITextMeasurer
{
public:
    virtual ~ITextMeasurer() = default;

    virtual uint32_t width(std::string_view text) const = 0;
};

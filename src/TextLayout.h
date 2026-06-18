#pragma once

#include <string>

#include <SDL3_ttf/SDL_ttf.h>



class TextLayout
{
public:
    TextLayout() = default;

    void setFont(TTF_Font* font);
    uint32_t width(const std::string& text) const;
    std::string expandTabs(std::string_view text) const ;
    uint32_t virtualColumn(std::string_view line, uint32_t rawCol);
    int columnToPixel(std::string_view line, uint32_t col) const;

private:
    TTF_Font *mFont;
};
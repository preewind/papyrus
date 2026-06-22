#include <gtest/gtest.h>

#include "theme.h"

namespace
{
void expectColor(const RenderColor &color, int r, int g, int b, int a)
{
    EXPECT_EQ(static_cast<int>(color.r), r);
    EXPECT_EQ(static_cast<int>(color.g), g);
    EXPECT_EQ(static_cast<int>(color.b), b);
    EXPECT_EQ(static_cast<int>(color.a), a);
}
} // namespace

TEST(Theme, DefaultPaletteMatchesExpectedValues)
{
    const Theme theme;

    expectColor(theme.background, 0, 0, 0, 255);
    expectColor(theme.text, 255, 255, 255, 255);
    expectColor(theme.cursor, 255, 255, 255, 255);
    expectColor(theme.selection, 46, 47, 108, 255);
    expectColor(theme.searchMatch, 46, 47, 108, 255);
    expectColor(theme.terminalBackground, 31, 32, 33, 255);
    expectColor(theme.terminalCursor, 101, 102, 103, 255);
    expectColor(theme.lineNumbers, 66, 67, 68, 255);
    expectColor(theme.overlayBackground, 34, 35, 36, 255);
    expectColor(theme.fileBrowserDir, 255, 255, 0, 255);
    expectColor(theme.fileBrowserUnsupported, 220, 80, 80, 255);
}

TEST(Theme, LexerDefaultPaletteMatchesExpectedValues)
{
    const LexerTheme lexerTheme;

    expectColor(lexerTheme.punctuation, 255, 255, 0, 255);
    expectColor(lexerTheme.comment, 139, 148, 158, 255);
    expectColor(lexerTheme.string, 165, 214, 255, 255);
    expectColor(lexerTheme.keyword, 255, 123, 114, 255);
    expectColor(lexerTheme.preprocessor, 197, 134, 192, 255);
    expectColor(lexerTheme.includeLib, 165, 214, 255, 255);
    expectColor(lexerTheme.Default, 255, 255, 255, 255);
}

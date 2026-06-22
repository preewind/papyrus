#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>

#include "util.h"
#include "types.h" // Contains your custom definitions like 'Range'

namespace
{
    // RAII helper to redirect std::cerr to a local stringstream
    class StreamRedirector
    {
    public:
        StreamRedirector(std::ostream &originalStream, std::stringstream &targetBuffer)
            : mOriginalStream(originalStream), mOriginalBuffer(originalStream.rdbuf(targetBuffer.rdbuf()))
        {
        }

        ~StreamRedirector()
        {
            mOriginalStream.rdbuf(mOriginalBuffer);
        }

    private:
        std::ostream &mOriginalStream;
        std::streambuf *mOriginalBuffer;
    };
}

TEST(UtilTests, CsfMacroPassesOnTrue)
{
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cerr, buffer);
        CSF(true);
    }
    EXPECT_TRUE(buffer.str().empty());
}

TEST(UtilTests, CsfMacroFailsAndExitsOnFalse)
{
    EXPECT_EXIT({ CSF(false); }, ::testing::ExitedWithCode(1), ".*SDL error in.*");
}

TEST(UtilTests, CspMacroPassesOnValidPointer)
{
    int dummy = 42;
    int *ptr = &dummy;

    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cerr, buffer);
        CSP(ptr);
    }
    EXPECT_TRUE(buffer.str().empty());
}

TEST(UtilTests, CspMacroLogsOnNullPointer)
{
    int *ptr = nullptr;
    std::stringstream buffer;
    {
        StreamRedirector redirect(std::cerr, buffer);
        CSP(ptr);
    }

    const std::string output = buffer.str();
    EXPECT_NE(output.find("[ERROR]"), std::string::npos);
    EXPECT_NE(output.find("SDL pointer error in"), std::string::npos);
}

TEST(UtilTests, SplitByNewlineHandlesVariousLineEndings)
{
    std::string input = "Line1\nLine2\r\nLine3";
    std::vector<std::string> result = splitByNewline(input);

    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "Line1");
    EXPECT_EQ(result[1], "Line2");
    EXPECT_EQ(result[2], "Line3");
}

TEST(UtilTests, SplitByNewlineEmptyInput)
{
    std::vector<std::string> result = splitByNewline("");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "");
}

TEST(UtilTests, HexCharToIntValid)
{
    EXPECT_EQ(hexCharToInt('0'), 0);
    EXPECT_EQ(hexCharToInt('9'), 9);
    EXPECT_EQ(hexCharToInt('a'), 10);
    EXPECT_EQ(hexCharToInt('F'), 15);
}

TEST(UtilTests, HexCharToIntInvalidCrashes)
{
    EXPECT_EXIT({ hexCharToInt('Z'); }, ::testing::ExitedWithCode(1), ".*Invalid color!.*");
}

TEST(UtilTests, HexToSdlColorParsing)
{
    SDL_Color c1 = hexToSDLColor("#FF00AA");
    EXPECT_EQ(c1.r, 255);
    EXPECT_EQ(c1.g, 0);
    EXPECT_EQ(c1.b, 170);
    EXPECT_EQ(c1.a, 255);

    SDL_Color c2 = hexToSDLColor("00FF007F");
    EXPECT_EQ(c2.r, 0);
    EXPECT_EQ(c2.g, 255);
    EXPECT_EQ(c2.b, 0);
    EXPECT_EQ(c2.a, 127);
}

TEST(UtilTests, HexToSdlColorFallbackOnInvalidLength)
{
    SDL_Color fallback = hexToSDLColor("#123");
    EXPECT_EQ(fallback.r, 255);
    EXPECT_EQ(fallback.g, 255);
    EXPECT_EQ(fallback.b, 255);
    EXPECT_EQ(fallback.a, 255);
}

TEST(UtilTests, TrimRemovesWhitespace)
{
    EXPECT_EQ(trim("   hello   "), "hello");
    EXPECT_EQ(trim("\t\n world \r"), "world");
    EXPECT_EQ(trim("   "), "");
}

TEST(UtilTests, FindWordLeftOfIndexFindsLastWord)
{
    Range r1 = findWordLeftOfIndex("hello world   ");
    EXPECT_EQ(r1.start, 6);
    EXPECT_EQ(r1.end, 11);

    Range r2 = findWordLeftOfIndex("   ");
    EXPECT_EQ(r2.start, 0);
    EXPECT_EQ(r2.end, 0);
}

TEST(UtilTests, FindWordRightOfIndexFindsFirstWord)
{
    Range r1 = findWordRightOfIndex("   hello world");
    EXPECT_EQ(r1.start, 3);
    EXPECT_EQ(r1.end, 8);

    Range r2 = findWordRightOfIndex("   ");
    EXPECT_EQ(r2.start, 0);
    EXPECT_EQ(r2.end, 0);
}
#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

#include "FileBrowser.h"
#include "FileBrowserView.h"
#include "TextLayout.h"
#include "ViewTestUtils.h"

namespace
{
class ScopedCurrentPath
{
public:
    explicit ScopedCurrentPath(const std::filesystem::path &newPath)
        : mOriginal(std::filesystem::current_path())
    {
        std::filesystem::current_path(newPath);
    }

    ~ScopedCurrentPath()
    {
        std::error_code ec;
        std::filesystem::current_path(mOriginal, ec);
    }

private:
    std::filesystem::path mOriginal;
};
} // namespace

class FileBrowserViewTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mRoot = std::filesystem::temp_directory_path() / "papyrus_filebrowser_view_tests";
        std::filesystem::remove_all(mRoot);
        std::filesystem::create_directories(mRoot);

        std::ofstream(mRoot / "very_long_filename_for_browser_rendering_test.txt") << "content";
        std::ofstream(mRoot / "short.md") << "content";

        mPathGuard = std::make_unique<ScopedCurrentPath>(mRoot);
    }

    void TearDown() override
    {
        mPathGuard.reset();
        std::error_code ec;
        std::filesystem::remove_all(mRoot, ec);
    }

    std::filesystem::path mRoot;
    std::unique_ptr<ScopedCurrentPath> mPathGuard;
};

TEST_F(FileBrowserViewTests, RenderUpdatesVisibleRowsAndTruncatesLongFileNames)
{
    FileBrowser browser;

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    FileBrowserLayout layout;
    layout.listViewport = {40, 60, 120, 200};
    layout.lineHeight = 20;
    layout.visibleRows = 4;
    layout.pathTextX = 40;
    layout.pathTextY = 20;
    layout.statusTextX = 40;
    layout.statusTextY = 40;
    layout.legendAnchorRightX = 220;
    layout.legendY = 20;
    layout.legendMarkerSize = 10;

    FakeRenderContext renderContext;

    FileBrowserView view;
    view.render(renderContext, browser, textLayout, layout);

    EXPECT_EQ(browser.getVisibleFiles(), layout.visibleRows);
    ASSERT_FALSE(renderContext.textCalls.empty());
    EXPECT_TRUE(std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [](const FakeRenderContext::TextCall &call)
                            { return call.text.find("...") != std::string::npos; }));
}

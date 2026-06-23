#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "FileBrowser.h"

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

class FileBrowserFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const auto unique = std::to_string(std::hash<std::string>{}(std::to_string(::testing::UnitTest::GetInstance()->random_seed()) +
                                                                     std::to_string(mCounter++)));
        mRoot = std::filesystem::temp_directory_path() / ("papyrus_filebrowser_tests_" + unique);
        std::filesystem::create_directories(mRoot);

        std::ofstream(mRoot / "Alpha.txt") << "alpha";
        std::ofstream(mRoot / "zeta.md") << "zeta";
        std::ofstream(mRoot / "image.bin") << "\x00\x01";
        std::filesystem::create_directory(mRoot / "docs");

        mPathGuard = std::make_unique<ScopedCurrentPath>(mRoot);
    }

    void TearDown() override
    {
        mPathGuard.reset();
        std::error_code ec;
        std::filesystem::remove_all(mRoot, ec);
    }

    static void selectEntryByName(FileBrowser &browser, const std::string &name)
    {
        const std::vector<std::string> items = browser.getCurrentDirFilesToRender();
        const auto it = std::find(items.begin(), items.end(), name);
        ASSERT_NE(it, items.end());

        const uint32_t target = static_cast<uint32_t>(std::distance(items.begin(), it));
        while (browser.getSelectedIndex() != target)
        {
            browser.handleDown();
        }
    }

    std::filesystem::path mRoot;

private:
    inline static size_t mCounter = 0;
    std::unique_ptr<ScopedCurrentPath> mPathGuard;
};
} // namespace

TEST_F(FileBrowserFixture, RendersParentEntryAndMarksOpenableEntries)
{
    FileBrowser browser;

    const std::vector<std::string> renderItems = browser.getCurrentDirFilesToRender();
    const std::vector<bool> openable = browser.getCurrentDirFilesOpenable();

    ASSERT_EQ(renderItems.size(), openable.size());
    ASSERT_GE(renderItems.size(), 5u);

    EXPECT_EQ(renderItems[0], "..");
    EXPECT_TRUE(openable[0]);

    auto alphaIt = std::find(renderItems.begin(), renderItems.end(), "Alpha.txt");
    auto zetaIt = std::find(renderItems.begin(), renderItems.end(), "zeta.md");
    auto docsIt = std::find(renderItems.begin(), renderItems.end(), "docs");
    auto binIt = std::find(renderItems.begin(), renderItems.end(), "image.bin");

    ASSERT_NE(alphaIt, renderItems.end());
    ASSERT_NE(zetaIt, renderItems.end());
    ASSERT_NE(docsIt, renderItems.end());
    ASSERT_NE(binIt, renderItems.end());

    EXPECT_TRUE(openable[static_cast<size_t>(std::distance(renderItems.begin(), alphaIt))]);
    EXPECT_TRUE(openable[static_cast<size_t>(std::distance(renderItems.begin(), zetaIt))]);
    EXPECT_TRUE(openable[static_cast<size_t>(std::distance(renderItems.begin(), docsIt))]);
    EXPECT_FALSE(openable[static_cast<size_t>(std::distance(renderItems.begin(), binIt))]);
}

TEST_F(FileBrowserFixture, UpAndDownWrapSelection)
{
    FileBrowser browser;
    const size_t fileCount = browser.getCurrentDirFiles().size();

    ASSERT_GT(fileCount, 0u);
    EXPECT_EQ(browser.getSelectedIndex(), 0u);

    browser.handleUp();
    EXPECT_EQ(browser.getSelectedIndex(), fileCount - 1);

    browser.handleDown();
    EXPECT_EQ(browser.getSelectedIndex(), 0u);
}

TEST_F(FileBrowserFixture, ReturnOnUnsupportedFileSetsErrorAndNoOpenRequest)
{
    FileBrowser browser;
    selectEntryByName(browser, "image.bin");

    browser.handleReturn();

    EXPECT_TRUE(browser.hasStatusError());
    EXPECT_EQ(browser.consumeOpenRequest(), std::nullopt);
    EXPECT_NE(browser.getStatusMessage().find("Unsupported file type"), std::string::npos);
}

TEST_F(FileBrowserFixture, ReturnOnTextFileQueuesOpenRequest)
{
    FileBrowser browser;
    selectEntryByName(browser, "Alpha.txt");

    browser.handleReturn();

    const auto firstRequest = browser.consumeOpenRequest();
    ASSERT_TRUE(firstRequest.has_value());
    EXPECT_EQ(firstRequest->filename().string(), "Alpha.txt");
    EXPECT_FALSE(browser.hasStatusError());

    const auto secondRequest = browser.consumeOpenRequest();
    EXPECT_FALSE(secondRequest.has_value());
}

TEST_F(FileBrowserFixture, ReturnOnDirectoryChangesCurrentDir)
{
    FileBrowser browser;
    selectEntryByName(browser, "docs");

    browser.handleReturn();

    EXPECT_FALSE(browser.hasStatusError());
    EXPECT_EQ(browser.getCurrentDir().filename().string(), "docs");
    EXPECT_EQ(browser.getSelectedIndex(), 0u);
}

TEST_F(FileBrowserFixture, VisibleRowsAdjustScrollOffsetWhenSelectionMoves)
{
    FileBrowser browser;
    ASSERT_GE(browser.getCurrentDirFiles().size(), 4u);
    browser.setVisibleFiles(2);

    browser.handleDown();
    browser.handleDown();
    browser.handleDown();

    EXPECT_EQ(browser.getSelectedIndex(), 3u);
    EXPECT_EQ(browser.getScrollOffset(), 2u);

    browser.handleUp();
    browser.handleUp();

    EXPECT_EQ(browser.getSelectedIndex(), 1u);
    EXPECT_EQ(browser.getScrollOffset(), 1u);
}

TEST_F(FileBrowserFixture, SelectionPathIsEmptyWhenDirectoryListIsInvalid)
{
    FileBrowser browser;

    std::filesystem::current_path(mRoot.parent_path());
    std::filesystem::remove_all(mRoot);

    browser.updateCurrentDirFiles();

    EXPECT_TRUE(browser.getCurrentDirFiles().empty());
    EXPECT_TRUE(browser.getSelectedIndexPath().empty());
    EXPECT_TRUE(browser.hasStatusError());
}

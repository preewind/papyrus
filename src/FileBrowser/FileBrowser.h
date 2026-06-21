#pragma once

#include <filesystem>
#include <vector>
#include <optional>
#include <string>

#include <SDL3/SDL_events.h>

class FileBrowser
{

public:
    FileBrowser() = default;
    void updateCurrentDirFiles();
    std::vector<std::filesystem::path> getCurrentDirFiles();
    std::vector<std::string> getCurrentDirFilesToRender();
    std::vector<bool> getCurrentDirFilesOpenable();
    const std::filesystem::path getCurrentDir() const;
    uint32_t getSelectedIndex() const;
    const std::filesystem::path getSelectedIndexPath() const;
    std::optional<std::filesystem::path> consumeOpenRequest();
    const std::string &getStatusMessage() const;
    bool hasStatusError() const;

    void handleKey(const SDL_Event &event);
    void handleReturn();
    void handleUp();
    void handleDown();

    void ensureSelectionVisible();
    void setVisibleFiles(uint32_t numFiles);
    const uint32_t &getVisibleFiles() const;
    const uint32_t &getScrollOffset() const;

    std::string getFileExtension(const std::filesystem::path &path) const;

private:
    bool isSupportedTextFile(const std::filesystem::path &path) const;
    bool isPathOpenable(const std::filesystem::path &path) const;
    void setStatus(std::string message, bool isError);

    std::vector<std::filesystem::path> mCurrentDirFiles;
    std::vector<bool> mCurrentDirFilesOpenable;
    std::filesystem::path mCurrentDir = std::filesystem::current_path();
    uint32_t mSelectedIndex = 0;
    std::optional<std::filesystem::path> mOpenRequest;
    uint32_t mScrollOffset = 0;
    uint32_t mVisibleFiles = 0;
    std::string mStatusMessage;
    bool mStatusIsError = false;
};
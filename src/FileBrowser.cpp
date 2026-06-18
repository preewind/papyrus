#include <algorithm>

#include "FileBrowser.h"
#include "logger.h"

void FileBrowser::updateCurrentDirFiles()
{
    std::vector<std::filesystem::path> files;
    std::filesystem::path currentDir = mCurrentDir;
    if (!std::filesystem::is_directory(currentDir))
        return;

    LOG_DEBUG() << "currDir: " << currentDir;

    // parent dir for .. navigation
    if (currentDir.has_parent_path())
    {
        files.push_back(currentDir.parent_path());
    }

    for (auto const &dir_entry : std::filesystem::directory_iterator{currentDir})
    {
        files.push_back(dir_entry.path());
    }
    // sort alphabetically, maybe add more sorting options in the future
    std::sort(files.begin() + 1, files.end(), [](const std::filesystem::path &a, const std::filesystem::path &b)
              {
        std::string strA = a.generic_string();
        std::string strB = b.generic_string();

    
        return std::lexicographical_compare(strA.begin(), strA.end(),strB.begin(), strB.end(),
            [](unsigned char charA, unsigned char charB) {
                return std::tolower(charA) < std::tolower(charB);
            }
        ); });
    mCurrentDirFiles = files;
}

std::vector<std::filesystem::path> FileBrowser::getCurrentDirFiles()
{
    // TODO smarter recalculation
    if (mCurrentDirFiles.size() == 0)
    {
        updateCurrentDirFiles();
    }
    return mCurrentDirFiles;
}

std::vector<std::string> FileBrowser::getCurrentDirFilesToRender()
{
    if (mCurrentDirFiles.size() == 0)
    {
        updateCurrentDirFiles();
    }
    std::vector<std::string> fileStrings;
    fileStrings.reserve(mCurrentDirFiles.size());

    for (size_t i = 0; i < mCurrentDirFiles.size(); i++)
    {
        // parent directory -> .. in rendering, except root path, TODO: test on Windows
        if (i == 0 && std::filesystem::is_directory(mCurrentDirFiles[i]) && mCurrentDirFiles[i] != mCurrentDir.root_directory())
        {
            fileStrings.push_back("..");
            continue;
        }
        else if (i == 0 && mCurrentDirFiles[i].parent_path() == mCurrentDir.root_directory() /*&& mCurrentDir != mCurrentDir.root_directory()*/)
        {
            fileStrings.push_back("..");
            continue;
        }
        fileStrings.push_back(mCurrentDirFiles[i].filename().string());
    }

    return fileStrings;
}

const std::filesystem::path FileBrowser::getCurrentDir() const
{
    return mCurrentDir;
}

uint32_t FileBrowser::getSelectedIndex() const
{
    return mSelectedIndex;
}

const std::filesystem::path FileBrowser::getSelectedIndexPath() const
{
    if (mCurrentDirFiles.size() == 0)
        return "";
    return mCurrentDirFiles[mSelectedIndex];
}

std::optional<std::filesystem::path> FileBrowser::consumeOpenRequest()
{
    std::optional<std::filesystem::path> result = mOpenRequest;
    mOpenRequest.reset();
    return result;
}

void FileBrowser::handleKey(const SDL_Event &event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        SDL_Keycode key = event.key.key;
        switch (key)
        {
        case SDLK_RETURN:
            handleReturn();
            break;
        case SDLK_UP:
            handleUp();
            break;
        case SDLK_DOWN:
            handleDown();
            break;
        }
    }
}

void FileBrowser::handleReturn()
{

    std::filesystem::path selectedPath = getSelectedIndexPath();

    bool hasSelectedDirectory = std::filesystem::is_directory(selectedPath);

    if (hasSelectedDirectory)
    {
        LOG_DEBUG() << "Selected Dir: " << selectedPath;
        mCurrentDir = selectedPath;
        mSelectedIndex = 0;
        mScrollOffset = 0;
        updateCurrentDirFiles();
    }
    else
    {
        mOpenRequest = selectedPath;
        LOG_DEBUG() << "Selected file: " << selectedPath;
    }
}

void FileBrowser::handleUp()
{
    mSelectedIndex = (mSelectedIndex + mCurrentDirFiles.size() - 1) % mCurrentDirFiles.size();
    ensureSelectionVisible();
    LOG_DEBUG() << "Index: " << mSelectedIndex;
}

void FileBrowser::handleDown()
{
    mSelectedIndex = (mSelectedIndex + 1) % mCurrentDirFiles.size();
    ensureSelectionVisible();
    LOG_DEBUG() << "Index: " << mSelectedIndex;
}

void FileBrowser::ensureSelectionVisible()
{
    if (mSelectedIndex < mScrollOffset)
    {
        mScrollOffset = mSelectedIndex;
    }
    else if (mSelectedIndex >= mScrollOffset + mVisibleFiles)
    {
        mScrollOffset = mSelectedIndex - mVisibleFiles + 1;
    }
    LOG_DEBUG() << "offset: " << mScrollOffset;
}

void FileBrowser::setVisibleFiles(uint32_t numFiles)
{
    mVisibleFiles = numFiles;
}

const uint32_t &FileBrowser::getVisibleFiles() const
{
    return mVisibleFiles;
}

const uint32_t &FileBrowser::getScrollOffset() const
{
    return mScrollOffset;
}

const std::string FileBrowser::getFileExtension(std::filesystem::path path) const
{
    return path.extension().string();
}

#include <algorithm>
#include <array>
#include <cctype>
#include <system_error>

#include "FileBrowser.h"
#include "logger.h"

void FileBrowser::updateCurrentDirFiles()
{
    std::vector<std::filesystem::path> files;
    std::vector<bool> openable;
    std::filesystem::path currentDir = mCurrentDir;
    std::error_code ec;
    if (!std::filesystem::is_directory(currentDir, ec) || ec)
    {
        setStatus("Could not read current directory.", true);
        mCurrentDirFiles.clear();
        mCurrentDirFilesOpenable.clear();
        return;
    }

    LOG_DEBUG() << "currDir: " << currentDir;

    // parent dir for .. navigation
    if (currentDir.has_parent_path() && currentDir != currentDir.root_path())
    {
        files.push_back(currentDir.parent_path());
        openable.push_back(true);
    }

    std::filesystem::directory_iterator iter(
        currentDir,
        std::filesystem::directory_options::skip_permission_denied,
        ec);
    if (ec)
    {
        setStatus("Could not list directory entries.", true);
    }

    for (auto const &dir_entry : iter)
    {
        files.push_back(dir_entry.path());
        openable.push_back(isPathOpenable(dir_entry.path()));
    }

    // sort alphabetically, maybe add more sorting options in the future
    const size_t sortStart = (files.empty() ? 0 : (files[0] == currentDir.parent_path() ? 1 : 0));
    std::vector<size_t> indices(files.size());
    for (size_t i = 0; i < indices.size(); ++i)
    {
        indices[i] = i;
    }

    if (sortStart < indices.size())
    {
        const auto sortOffset = static_cast<std::vector<size_t>::difference_type>(sortStart);
        std::sort(indices.begin() + sortOffset, indices.end(), [&files](size_t ia, size_t ib)
                  {
        std::string strA = files[ia].generic_string();
        std::string strB = files[ib].generic_string();


        return std::lexicographical_compare(strA.begin(), strA.end(),strB.begin(), strB.end(),
            [](unsigned char charA, unsigned char charB) {
                return std::tolower(charA) < std::tolower(charB);
            }
        ); });
    }

    std::vector<std::filesystem::path> sortedFiles;
    std::vector<bool> sortedOpenable;
    sortedFiles.reserve(files.size());
    sortedOpenable.reserve(openable.size());
    for (size_t index : indices)
    {
        sortedFiles.push_back(files[index]);
        sortedOpenable.push_back(openable[index]);
    }

    mCurrentDirFiles = std::move(sortedFiles);
    mCurrentDirFilesOpenable = std::move(sortedOpenable);

    if (mSelectedIndex >= mCurrentDirFiles.size())
    {
        mSelectedIndex = 0;
    }
    if (mCurrentDirFiles.empty())
    {
        mScrollOffset = 0;
    }
    else
    {
        ensureSelectionVisible();
    }

    if (!mStatusIsError)
    {
        setStatus("Use Up/Down + Enter to browse.", false);
    }
}

const std::vector<std::filesystem::path>& FileBrowser::getCurrentDirFiles()
{
    // TODO smarter recalculation
    if (mCurrentDirFiles.empty())
    {
        updateCurrentDirFiles();
    }
    return mCurrentDirFiles;
}

std::vector<std::string> FileBrowser::getCurrentDirFilesToRender()
{
    if (mCurrentDirFiles.empty())
    {
        updateCurrentDirFiles();
    }
    std::vector<std::string> fileStrings;
    fileStrings.reserve(mCurrentDirFiles.size());

    for (size_t i = 0; i < mCurrentDirFiles.size(); i++)
    {
        // parent directory -> .. in rendering
        if (i == 0 && mCurrentDir != mCurrentDir.root_path() && mCurrentDirFiles[i] == mCurrentDir.parent_path())
        {
            fileStrings.push_back("..");
            continue;
        }

        fileStrings.push_back(mCurrentDirFiles[i].filename().string());
    }

    return fileStrings;
}

const std::vector<bool>& FileBrowser::getCurrentDirFilesOpenable()
{
    if (mCurrentDirFilesOpenable.size() != mCurrentDirFiles.size())
    {
        updateCurrentDirFiles();
    }
    return mCurrentDirFilesOpenable;
}

const std::filesystem::path& FileBrowser::getCurrentDir() const
{
    return mCurrentDir;
}

uint32_t FileBrowser::getSelectedIndex() const
{
    return mSelectedIndex;
}

std::filesystem::path FileBrowser::getSelectedIndexPath() const
{
    if (mCurrentDirFiles.empty())
        return std::string("");
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
    if (mCurrentDirFiles.empty() || mSelectedIndex >= mCurrentDirFiles.size())
    {
        setStatus("No selectable entry in this directory.", true);
        return;
    }

    std::filesystem::path selectedPath = getSelectedIndexPath();

    std::error_code ec;
    bool hasSelectedDirectory = std::filesystem::is_directory(selectedPath, ec);
    if (ec)
    {
        setStatus("Unable to access selected path.", true);
        return;
    }

    if (hasSelectedDirectory)
    {
        LOG_DEBUG() << "Selected Dir: " << selectedPath;
        mCurrentDir = selectedPath;
        mSelectedIndex = 0;
        mScrollOffset = 0;
        setStatus("Directory: " + selectedPath.string(), false);
        updateCurrentDirFiles();
    }
    else
    {
        if (!isPathOpenable(selectedPath))
        {
            setStatus("Unsupported file type: " + selectedPath.filename().string(), true);
            return;
        }

        mOpenRequest = selectedPath;
        setStatus("Opening: " + selectedPath.filename().string(), false);
        LOG_DEBUG() << "Selected file: " << selectedPath;
    }
}

void FileBrowser::handleUp()
{
    if (mCurrentDirFiles.empty())
    {
        return;
    }

    mSelectedIndex = (mSelectedIndex + mCurrentDirFiles.size() - 1) % mCurrentDirFiles.size();
    ensureSelectionVisible();
    LOG_DEBUG() << "Index: " << mSelectedIndex;
}

void FileBrowser::handleDown()
{
    if (mCurrentDirFiles.empty())
    {
        return;
    }

    mSelectedIndex = (mSelectedIndex + 1) % mCurrentDirFiles.size();
    ensureSelectionVisible();
    LOG_DEBUG() << "Index: " << mSelectedIndex;
}

void FileBrowser::ensureSelectionVisible()
{
    if (mCurrentDirFiles.empty() || mVisibleFiles == 0)
    {
        mScrollOffset = 0;
        return;
    }

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

std::string FileBrowser::getFileExtension(const std::filesystem::path &path) const
{
    return path.extension().string();
}

const std::string &FileBrowser::getStatusMessage() const
{
    return mStatusMessage;
}

bool FileBrowser::hasStatusError() const
{
    return mStatusIsError;
}

bool FileBrowser::isSupportedTextFile(const std::filesystem::path &path) const
{
    const std::string ext = getFileExtension(path);
    if (!ext.empty())
    {
        std::string lowerExt = ext;
        std::transform(lowerExt.begin(), lowerExt.end(), lowerExt.begin(), [](unsigned char c)
                       { return static_cast<char>(std::tolower(c)); });

        static const std::array<std::string, 29> allowedExtensions = {
            ".txt", ".md", ".markdown", ".json", ".yaml", ".yml", ".toml", ".ini", ".cfg", ".conf",
            ".log", ".csv", ".tsv", ".xml", ".html", ".htm", ".css", ".js", ".ts", ".py",
            ".c", ".cc", ".cpp", ".h", ".hpp", ".java", ".go", ".rs", ".sh"};

        return std::find(allowedExtensions.begin(), allowedExtensions.end(), lowerExt) != allowedExtensions.end();
    }

    const std::string fileName = path.filename().string();
    static const std::array<std::string, 5> allowedNoExt = {
        "README", "LICENSE", "Makefile", "CMakeLists.txt", ".gitignore"};

    return std::find(allowedNoExt.begin(), allowedNoExt.end(), fileName) != allowedNoExt.end();
}

bool FileBrowser::isPathOpenable(const std::filesystem::path &path) const
{
    std::error_code ec;
    if (std::filesystem::is_directory(path, ec))
    {
        return true;
    }
    ec.clear();
    if (!std::filesystem::is_regular_file(path, ec) || ec)
    {
        return false;
    }
    return isSupportedTextFile(path);
}

void FileBrowser::setStatus(std::string message, bool isError)
{
    mStatusMessage = std::move(message);
    mStatusIsError = isError;
}

#include "FileBrowserView.h"
#include "Editor.h"

const std::string FileBrowserView::fitTextToWidthFile(const std::string &text, const std::string &extension, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps) const
{
    uint32_t visibleWidth = sdlProps.totalWindowWidth - layoutConfig.editorMarginLeft - textLayout.width("...") - textLayout.width(extension);

    if (textLayout.width(text) - textLayout.width(extension) <= visibleWidth)
    {
        return text;
    }

    uint32_t low = 0;
    uint32_t high = text.length() - extension.size();
    uint32_t bestLength = 0;

    while (low <= high)
    {
        uint32_t mid = low + (high - low) / 2;
        std::string subs = text.substr(0, mid);

        if (textLayout.width(subs) <= visibleWidth)
        {
            bestLength = mid;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return text.substr(0, bestLength) + "..." + extension;
}

int FileBrowserView::screenY(uint32_t row, uint32_t scrollOffset, uint32_t margin, uint32_t lineHeight) const
{
    return margin + (row - scrollOffset) * lineHeight;
}

void FileBrowserView::render(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    renderFileBrowser(renderContext, browser, textLayout, layoutConfig, sdlProps);
}

void FileBrowserView::renderFileBrowserSelection(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    const auto &theme = renderContext.getTheme();
    const std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    int x = layoutConfig.editorMarginLeft;
    int y = screenY(browser.getSelectedIndex(), browser.getScrollOffset(), layoutConfig.editorMarginTop + (sdlProps.lineHeight * 2), sdlProps.lineHeight);
    int w = textLayout.width(filesToRender[browser.getSelectedIndex()]);
    int h = sdlProps.lineHeight;
    renderContext.drawRect(x, y, w, h, theme.selection);
}

void FileBrowserView::renderFileBrowser(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    const auto &theme = renderContext.getTheme();
    renderFileBrowserSelection(renderContext, browser, textLayout, layoutConfig, sdlProps);
    std::string currentPathStr = browser.getCurrentDir().string();
    renderContext.drawText(currentPathStr, layoutConfig.editorMarginLeft, layoutConfig.editorMarginTop);

    uint32_t fileListTopMargin = layoutConfig.editorMarginTop + (sdlProps.lineHeight * 2);

    uint32_t visibleFiles = (sdlProps.totalWindowHeight - fileListTopMargin) / sdlProps.lineHeight;
    browser.setVisibleFiles(visibleFiles);
    RenderColor color;
    std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    uint32_t first = browser.getScrollOffset();
    uint32_t last = std::min(static_cast<int>(first + visibleFiles), static_cast<int>(filesToRender.size()));
    for (size_t i = first; i < last; ++i)
    {
        std::string file = filesToRender[i];
        color = theme.text;
        if (std::filesystem::is_directory(currentPathStr / std::filesystem::path{file}))
        {
            color = theme.fileBrowserDir;
        }
        std::string extension = browser.getFileExtension(file);
        uint32_t first = browser.getScrollOffset();
        file = fitTextToWidthFile(file, extension, textLayout, layoutConfig, sdlProps);
        renderContext.drawText(file, layoutConfig.editorMarginLeft, screenY(i, first, fileListTopMargin, sdlProps.lineHeight), color);
    }
}
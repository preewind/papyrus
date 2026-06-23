#include <algorithm>

#include "FileBrowserView.h"

std::string FileBrowserView::fitTextToWidthFile(const std::string &text, const std::string &extension, const TextLayout &textLayout, uint32_t availableWidth) const
{
    const uint32_t reservedWidth = textLayout.width("...") + textLayout.width(extension);
    if (availableWidth <= reservedWidth)
    {
        return "...";
    }

    uint32_t visibleWidth = availableWidth - reservedWidth;

    if (textLayout.width(text) - textLayout.width(extension) <= visibleWidth)
    {
        return text;
    }

    uint32_t low = 0;
    uint32_t high = static_cast<uint32_t>(text.length() > extension.size() ? text.length() - extension.size() : text.length());
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

int FileBrowserView::screenY(uint32_t row, uint32_t scrollOffset, uint32_t listTop, uint32_t lineHeight) const
{
    return listTop + (row - scrollOffset) * lineHeight;
}

void FileBrowserView::render(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const FileBrowserLayout &layout)
{
    renderFileBrowser(renderContext, browser, textLayout, layout);
}

void FileBrowserView::renderFileBrowserSelection(RenderContext &renderContext, FileBrowser &browser, const FileBrowserLayout &layout)
{
    const auto &theme = renderContext.getTheme();
    const std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    if (filesToRender.empty() || browser.getSelectedIndex() >= filesToRender.size())
    {
        return;
    }

    int x = static_cast<int>(layout.listViewport.x);
    int y = screenY(browser.getSelectedIndex(), browser.getScrollOffset(), layout.listViewport.y, layout.lineHeight);
    int w = static_cast<int>(layout.listViewport.w);
    int h = static_cast<int>(layout.lineHeight);
    renderContext.drawRect(x, y, w, h, theme.selection);
}

void FileBrowserView::renderFileBrowser(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const FileBrowserLayout &layout)
{
    const auto &theme = renderContext.getTheme();
    std::string currentPathStr = browser.getCurrentDir().string();
    renderContext.drawText(currentPathStr, layout.pathTextX, layout.pathTextY);

    const std::string legendLabel = "unsupported";
    const int legendSpacing = 8;
    const int markerSize = static_cast<int>(layout.legendMarkerSize);
    const int legendWidth = markerSize + legendSpacing + textLayout.width(legendLabel);
    int legendX = static_cast<int>(layout.legendAnchorRightX) - legendWidth;
    legendX = std::max(static_cast<int>(layout.pathTextX), legendX);
    const int legendY = static_cast<int>(layout.legendY);

    renderContext.drawRect(legendX, legendY + (static_cast<int>(layout.lineHeight) - markerSize) / 2, markerSize, markerSize, theme.fileBrowserUnsupported);
    renderContext.drawText(legendLabel, legendX + markerSize + legendSpacing, legendY, theme.text);

    const std::string &status = browser.getStatusMessage();
    RenderColor statusColor = browser.hasStatusError() ? theme.fileBrowserUnsupported : theme.text;
    if (!status.empty())
    {
        renderContext.drawText(status, layout.statusTextX, layout.statusTextY, statusColor);
    }

    browser.setVisibleFiles(layout.visibleRows);

    std::vector<std::filesystem::path> currentDirFiles = browser.getCurrentDirFiles();
    std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    std::vector<bool> openable = browser.getCurrentDirFilesOpenable();

    renderFileBrowserSelection(renderContext, browser, layout);

    RenderColor color;
    uint32_t first = browser.getScrollOffset();
    uint32_t last = std::min(static_cast<int>(first + layout.visibleRows), static_cast<int>(filesToRender.size()));
    for (size_t i = first; i < last; ++i)
    {
        std::string file = filesToRender[i];
        color = theme.text;
        if (i < openable.size() && !openable[i])
        {
            color = theme.fileBrowserUnsupported;
        }
        else if (i < currentDirFiles.size())
        {
            std::error_code ec;
            if (std::filesystem::is_directory(currentDirFiles[i], ec) && !ec)
            {
                color = theme.fileBrowserDir;
            }
        }
        std::string extension = browser.getFileExtension(filesToRender[i]);
        uint32_t first = browser.getScrollOffset();
        file = fitTextToWidthFile(file, extension, textLayout, layout.listViewport.w);
        renderContext.drawText(file, layout.listViewport.x, screenY(i, first, layout.listViewport.y, layout.lineHeight), color);
    }
}
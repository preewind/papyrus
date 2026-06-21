#include <algorithm>

#include "FileBrowserView.h"

const std::string FileBrowserView::fitTextToWidthFile(const std::string &text, const std::string &extension, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps) const
{
    const uint32_t reservedWidth = layoutConfig.editorMarginLeft + layoutConfig.editorMarginRight + textLayout.width("...") + textLayout.width(extension);
    if (sdlProps.totalWindowWidth <= reservedWidth)
    {
        return "...";
    }

    uint32_t visibleWidth = sdlProps.totalWindowWidth - reservedWidth;

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

int FileBrowserView::screenY(uint32_t row, uint32_t scrollOffset, uint32_t margin, uint32_t lineHeight) const
{
    return margin + (row - scrollOffset) * lineHeight;
}

void FileBrowserView::render(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    renderFileBrowser(renderContext, browser, textLayout, layoutConfig, sdlProps);
}

void FileBrowserView::renderFileBrowserSelection(RenderContext &renderContext, FileBrowser &browser, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    const auto &theme = renderContext.getTheme();
    const std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    if (filesToRender.empty() || browser.getSelectedIndex() >= filesToRender.size())
    {
        return;
    }

    int x = layoutConfig.editorMarginLeft;
    int y = screenY(browser.getSelectedIndex(), browser.getScrollOffset(), layoutConfig.editorMarginTop + (sdlProps.lineHeight * 2), sdlProps.lineHeight);
    int w = static_cast<int>(sdlProps.totalWindowWidth);
    w -= static_cast<int>(layoutConfig.editorMarginLeft + layoutConfig.editorMarginRight);
    w = std::max(0, w);
    int h = sdlProps.lineHeight;
    renderContext.drawRect(x, y, w, h, theme.selection);
}

void FileBrowserView::renderFileBrowser(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    const auto &theme = renderContext.getTheme();
    std::string currentPathStr = browser.getCurrentDir().string();
    renderContext.drawText(currentPathStr, layoutConfig.editorMarginLeft, layoutConfig.editorMarginTop);

    const std::string legendLabel = "unsupported";
    const int markerSize = std::max(8, static_cast<int>(sdlProps.lineHeight / 2));
    const int legendSpacing = 8;
    const int legendPaddingRight = static_cast<int>(layoutConfig.editorMarginRight);
    const int legendWidth = markerSize + legendSpacing + textLayout.width(legendLabel);
    int legendX = static_cast<int>(sdlProps.totalWindowWidth) - legendPaddingRight - legendWidth;
    legendX = std::max(static_cast<int>(layoutConfig.editorMarginLeft), legendX);
    const int legendY = static_cast<int>(layoutConfig.editorMarginTop);

    renderContext.drawRect(legendX, legendY + (static_cast<int>(sdlProps.lineHeight) - markerSize) / 2, markerSize, markerSize, theme.fileBrowserUnsupported);
    renderContext.drawText(legendLabel, legendX + markerSize + legendSpacing, legendY, theme.text);

    const std::string &status = browser.getStatusMessage();
    RenderColor statusColor = browser.hasStatusError() ? theme.fileBrowserUnsupported : theme.text;
    if (!status.empty())
    {
        renderContext.drawText(status, layoutConfig.editorMarginLeft, layoutConfig.editorMarginTop + static_cast<int>(sdlProps.lineHeight), statusColor);
    }

    uint32_t fileListTopMargin = layoutConfig.editorMarginTop + (sdlProps.lineHeight * 2);

    uint32_t visibleFiles = 0;
    if (sdlProps.totalWindowHeight > fileListTopMargin && sdlProps.lineHeight > 0)
    {
        visibleFiles = (sdlProps.totalWindowHeight - fileListTopMargin) / sdlProps.lineHeight;
    }
    browser.setVisibleFiles(visibleFiles);

    std::vector<std::filesystem::path> currentDirFiles = browser.getCurrentDirFiles();
    std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    std::vector<bool> openable = browser.getCurrentDirFilesOpenable();

    renderFileBrowserSelection(renderContext, browser, layoutConfig, sdlProps);

    RenderColor color;
    uint32_t first = browser.getScrollOffset();
    uint32_t last = std::min(static_cast<int>(first + visibleFiles), static_cast<int>(filesToRender.size()));
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
        file = fitTextToWidthFile(file, extension, textLayout, layoutConfig, sdlProps);
        renderContext.drawText(file, layoutConfig.editorMarginLeft, screenY(i, first, fileListTopMargin, sdlProps.lineHeight), color);
    }
}
#include "FileBrowserView.h"
#include "Renderer.h"
#include "Editor.h"

void FileBrowserView::render(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    renderFileBrowser(renderer, browser, textLayout, layoutConfig, sdlProps);
}

void FileBrowserView::renderFileBrowserSelection(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    const auto &theme = renderer.getTheme();
    const std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    int x = layoutConfig.editorMarginLeft;
    int y = renderer.screenYBrowser(browser.getSelectedIndex(), browser.getScrollOffset(), layoutConfig.editorMarginTop + (sdlProps.lineHeight * 2));
    int w = textLayout.width(filesToRender[browser.getSelectedIndex()]);
    int h = sdlProps.lineHeight;
    renderer.drawRect(x, y, w, h, theme.selection);
}

void FileBrowserView::renderFileBrowser(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout, const LayoutConfig &layoutConfig, const SDL_Properties &sdlProps)
{
    const auto &theme = renderer.getTheme();
    renderFileBrowserSelection(renderer, browser, textLayout, layoutConfig, sdlProps);
    std::string currentPathStr = browser.getCurrentDir().string();
    renderer.drawText(currentPathStr, layoutConfig.editorMarginLeft, layoutConfig.editorMarginTop);

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
        file = renderer.fitTextToWidthFile(file, extension, layoutConfig);
        renderer.drawText(file, layoutConfig.editorMarginLeft, renderer.screenYBrowser(i, first, fileListTopMargin), color);
    }
}
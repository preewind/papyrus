#include "FileBrowserView.h"
#include "Renderer.h"
#include "Editor.h"

void FileBrowserView::render(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout)
{
    renderFileBrowser(renderer, browser, textLayout);
}

void FileBrowserView::renderFileBrowserSelection(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout)
{
    const auto &layout = renderer.getSDL_Properties();
    const auto &theme = renderer.getTheme();
    const auto &layoutConfig = renderer.getLayoutConfig();
    const std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    int x = layoutConfig.editorMarginLeft;
    int y = renderer.screenYBrowser(browser.getSelectedIndex(), browser.getScrollOffset(), layoutConfig.editorMarginTop + (layout.lineHeight * 2));
    int w = textLayout.width(filesToRender[browser.getSelectedIndex()]);
    int h = layout.lineHeight;
    renderer.drawRect(x, y, w, h, theme.selection);
}

void FileBrowserView::renderFileBrowser(Renderer &renderer, FileBrowser &browser, const TextLayout &textLayout)
{
    const auto &layout = renderer.getSDL_Properties();
    const auto &layoutConfig = renderer.getLayoutConfig();
    const auto &theme = renderer.getTheme();
    renderFileBrowserSelection(renderer, browser, textLayout);
    std::string currentPathStr = browser.getCurrentDir().string();
    renderer.drawText(currentPathStr, layoutConfig.editorMarginLeft, layoutConfig.editorMarginTop);

    uint32_t fileListTopMargin = layoutConfig.editorMarginTop + (layout.lineHeight * 2);

    uint32_t visibleFiles = (layout.totalWindowHeight - fileListTopMargin) / layout.lineHeight;
    browser.setVisibleFiles(visibleFiles);
    SDL_Color color;
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
        file = renderer.fitTextToWidthFile(file, extension);
        renderer.drawText(file, layoutConfig.editorMarginLeft, renderer.screenYBrowser(i, first, fileListTopMargin), color);
    }
}
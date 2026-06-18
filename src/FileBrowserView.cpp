#include "FileBrowserView.h"
#include "Renderer.h"

void FileBrowserView::render(Renderer &renderer, FileBrowser &browser)
{
    renderFileBrowser(renderer, browser);
}

void FileBrowserView::renderFileBrowserSelection(Renderer &renderer, FileBrowser &browser)
{
    const auto &layout = renderer.getEditorLayout();
    const auto &theme = renderer.getTheme();
    const auto &textLayout = renderer.getTextLayout();
    const std::vector<std::string> filesToRender = browser.getCurrentDirFilesToRender();
    int x = layout.marginLeft;
    int y = renderer.screenYBrowser(browser.getSelectedIndex(), browser.getScrollOffset(), layout.marginTop + (layout.lineHeight * 2));
    int w = textLayout.width(filesToRender[browser.getSelectedIndex()]);
    int h = layout.lineHeight;
    renderer.drawRect(x, y, w, h, theme.selection);
}

void FileBrowserView::renderFileBrowser(Renderer &renderer, FileBrowser &browser)
{
    const auto &layout = renderer.getEditorLayout();
    const auto &theme = renderer.getTheme();
    renderFileBrowserSelection(renderer, browser);
    std::string currentPathStr = browser.getCurrentDir().string();
    renderer.drawText(currentPathStr, layout.marginLeft, layout.marginTop);

    uint32_t fileListTopMargin = layout.marginTop + (layout.lineHeight * 2);

    uint32_t visibleFiles = (layout.windowHeight - fileListTopMargin) / layout.lineHeight;
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
        renderer.drawText(file, layout.marginLeft, renderer.screenYBrowser(i, first, fileListTopMargin), color);
    }
}
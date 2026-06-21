#pragma once

#include "FileBrowser.h"
#include "LayoutManager.h"
#include "RenderContext.h"

#include "TextLayout.h"

class FileBrowserView
{
public:
    void render(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const FileBrowserLayout &layout);

private:
    const std::string fitTextToWidthFile(const std::string &text, const std::string &extension, const TextLayout &textLayout, uint32_t availableWidth) const;
    int screenY(uint32_t row, uint32_t scrollOffset, uint32_t listTop, uint32_t lineHeight) const;
    void renderFileBrowserSelection(RenderContext &renderContext, FileBrowser &browser, const FileBrowserLayout &layout);
    void renderFileBrowser(RenderContext &renderContext, FileBrowser &browser, const TextLayout &textLayout, const FileBrowserLayout &layout);
};
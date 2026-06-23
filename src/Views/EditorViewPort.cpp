#include "EditorViewPort.h"
#include "Editor.h"

void EditorViewport::updateHorizontal(const Editor &editor, const TextLayout &textLayout, const LayoutConfig &layout, const LayoutInput &input)
{
    Cursor cursor = editor.getCursor();
    std::string line = editor.getLineString(cursor.row);

    int cursorPixelX = textLayout.columnToPixel(line, cursor.col);
    mViewport.visibleWidth = input.windowWidth - layout.editorMarginLeft - layout.lineNumberWidth;
    mViewport.ensureVisible(cursorPixelX, 20);
}

void EditorViewport::updateVertical(const Editor &editor, uint32_t visibleRows)
{
    mViewport.visibleRows = visibleRows;
    Cursor cursor = editor.getCursor();
    mViewport.ensureVisibleRow(cursor.row);
}

int EditorViewport::scrollX() const
{
    return mViewport.offsetX;
}

uint32_t EditorViewport::scrollY() const
{
    return mViewport.offsetY;
}
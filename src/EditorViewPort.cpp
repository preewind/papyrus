#include "EditorViewPort.h"
#include "Editor.h"

void EditorViewport::updateHorizontal(const Editor &editor, const TextLayout &textLayout, const LayoutConfig &layout, const LayoutInput &input)
{
    Cursor cursor = editor.getCursor();
    std::string line = editor.getLineString(cursor.row);

    int cursorPixelX = textLayout.columnToPixel(line, cursor.col);
    mHorizontal.visibleWidth = input.windowWidth - layout.editorMarginLeft - layout.lineNumberWidth;
    mHorizontal.ensureVisible(cursorPixelX, 20);
}

void EditorViewport::updateVertical(const Editor &editor, uint32_t visibleRows)
{
    mVisibleRows = visibleRows;
    Cursor cursor = editor.getCursor();

    if (cursor.row < mScrollOffsetY)
    {
        mScrollOffsetY = cursor.row;
    }
    else if (cursor.row >= mScrollOffsetY + mVisibleRows)
    {
        mScrollOffsetY = cursor.row - mVisibleRows + 1;
    }
}

int EditorViewport::scrollX() const
{
    return mHorizontal.offsetX;
}

uint32_t EditorViewport::scrollY() const
{
    return mScrollOffsetY;
}
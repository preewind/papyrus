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

int EditorViewport::scrollX() const
{
    return mHorizontal.offsetX;
}
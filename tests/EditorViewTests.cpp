#include <gtest/gtest.h>

#include <algorithm>

#include "Editor.h"
#include "EditorView.h"
#include "EditorViewPort.h"
#include "TextLayout.h"
#include "ViewTestUtils.h"

TEST(EditorViewTests, RenderDrawsLineNumbersTextAndCursor)
{
    Editor editor;
    editor.handleTextInput("alpha");
    editor.handleReturn();
    editor.handleTextInput("beta");
    editor.setVisibleRows(8);

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    LayoutConfig layoutConfig{};
    LayoutInput layoutInput{800, 600, 20};
    EditorLayout editorLayout{{0, 0, 800, 600}};

    EditorViewport viewport;
    viewport.updateHorizontal(editor, textLayout, layoutConfig, layoutInput);
    viewport.updateVertical(editor, editor.getVisibleRows());

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    EditorView view;
    view.render(renderContext, editor, viewport, textLayout, layoutConfig, editorLayout, true);

    ASSERT_FALSE(renderContext.textCalls.empty());
    EXPECT_TRUE(std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [](const FakeRenderContext::TextCall &call)
                            { return call.text == "1"; }));
    EXPECT_TRUE(std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [](const FakeRenderContext::TextCall &call)
                            { return call.text.find("beta") != std::string::npos; }));

    EXPECT_TRUE(std::any_of(renderContext.rectCalls.begin(), renderContext.rectCalls.end(), [](const FakeRenderContext::RectCall &call)
                            { return call.w == 2 && call.h == 20; }));
    EXPECT_EQ(renderContext.clipRects.size(), 1u);
    EXPECT_EQ(renderContext.clearClipCount, 1u);
}

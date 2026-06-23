#include <gtest/gtest.h>

#include <algorithm>

#include "Editor.h"
#include "SearchView.h"
#include "SearchViewPort.h"
#include "TextLayout.h"
#include "ViewTestUtils.h"

TEST(SearchViewTests, RenderSkipsWhenSearchIsInactive)
{
    Editor editor;

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    SearchLayout searchLayout{{300, 40, 350, 30}, {655, 40, 100, 30}, 305, 45, 660, 10, 5, 5};
    SearchViewport viewport;

    FakeRenderContext renderContext;
    SearchView view;
    view.render(renderContext, editor, textLayout, searchLayout, viewport, true);

    EXPECT_TRUE(renderContext.textCalls.empty());
    EXPECT_TRUE(renderContext.rectCalls.empty());
}

TEST(SearchViewTests, RenderDrawsOverlayQueryAndMatchCounterWhenActive)
{
    Editor editor;
    editor.handleTextInput("find this find");
    editor.handleF(SDL_KMOD_CTRL);
    editor.handleKey(makeTextInput("find"));
    editor.updateSearchMatches();

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    SearchLayout searchLayout{{300, 40, 350, 30}, {655, 40, 100, 30}, 305, 45, 660, 10, 5, 5};
    SearchViewport viewport;
    viewport.updateHorizontal(editor.getSearch(), textLayout, searchLayout);

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    SearchView view;
    view.render(renderContext, editor, textLayout, searchLayout, viewport, true);

    ASSERT_FALSE(renderContext.rectCalls.empty());
    EXPECT_TRUE(std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [](const FakeRenderContext::TextCall &call)
                            { return call.text == "find"; }));
    EXPECT_TRUE(std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [](const FakeRenderContext::TextCall &call)
                            { return call.text.find("/") != std::string::npos; }));
    EXPECT_TRUE(std::any_of(renderContext.rectCalls.begin(), renderContext.rectCalls.end(), [](const FakeRenderContext::RectCall &call)
                            { return call.w == 2; }));
}

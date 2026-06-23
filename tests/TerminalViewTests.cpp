#include <gtest/gtest.h>

#include <algorithm>

#include "Editor.h"
#include "TerminalView.h"
#include "TextLayout.h"
#include "ViewTestUtils.h"

TEST(TerminalViewTests, RenderSkipsWhenTerminalIsHidden)
{
    Editor editor;

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    TerminalLayout terminalLayout{{0, 480, 800, 120}, 10, 10};

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    TerminalView view;
    view.render(renderContext, editor, textLayout, terminalLayout, renderContext.windowProperties);

    EXPECT_TRUE(renderContext.textCalls.empty());
    EXPECT_TRUE(renderContext.rectCalls.empty());
}

TEST(TerminalViewTests, RenderDrawsPromptAndTerminalFrameWhenVisible)
{
    Editor editor;
    editor.handleT(SDL_KMOD_CTRL);
    editor.getTerminal().setVisibleRows(4);
    editor.getTerminal().handleKey(makeTextInput("echo hello"));

    TextLayout textLayout;
    FixedWidthMeasurer measurer(8);
    textLayout.setMeasurer(&measurer);

    TerminalLayout terminalLayout{{0, 480, 800, 120}, 10, 10};

    FakeRenderContext renderContext;
    renderContext.windowProperties = {20, 800, 600};

    TerminalView view;
    view.render(renderContext, editor, textLayout, terminalLayout, renderContext.windowProperties);

    ASSERT_FALSE(renderContext.rectCalls.empty());
    EXPECT_TRUE(std::any_of(renderContext.textCalls.begin(), renderContext.textCalls.end(), [](const FakeRenderContext::TextCall &call)
                            { return call.text.find("$ echo hello") != std::string::npos; }));
}

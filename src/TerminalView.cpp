#include "TerminalView.h"
#include "Renderer.h"

void TerminalView::render(Renderer &renderer, const Editor &editor)
{
    if (editor.isTerminalVisible())
    {
        renderTerminal(renderer, editor);
    }
}

void TerminalView::renderTerminal(Renderer &renderer, const Editor &editor)
{
    const auto &layout = renderer.getTerminalLayout();
    const auto &editorLayout = renderer.getEditorLayout();
    const auto &theme = renderer.getTheme();
    renderer.drawRect(layout.windowX, layout.windowY, editorLayout.windowWidth, layout.windowHeight, theme.terminalBackground);
    const Terminal &terminal = editor.getTerminalConst();
    renderTerminalCursor(renderer, terminal);
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    std::vector<std::string> output = terminal.getOutput().getText();
    std::reverse(output.begin(), output.end());
    if (output.size() == 0)
        return;
    uint32_t visRows = terminal.getVisibleRows();
    uint32_t first = terminal.getScrollOffset();
    uint32_t last = std::min(visRows, (uint32_t)output.size());
    for (uint32_t i = 0; i < last; ++i)
    {
        renderer.drawText(output[first + i], layout.windowX + layout.marginLeft, editorLayout.totalWindowHeight - layout.marginTop - (i + 2) * editorLayout.lineHeight);
    }

    renderer.drawText(text, layout.windowX + layout.marginLeft, editorLayout.totalWindowHeight - layout.marginTop - editorLayout.lineHeight);
}

void TerminalView::renderTerminalCursor(Renderer &renderer, const Terminal &terminal)
{
    const auto &layout = renderer.getTerminalLayout();
    const auto &editorLayout = renderer.getEditorLayout();
    const auto &theme = renderer.getTheme();
    const auto &textLayout = renderer.getTextLayout();
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    uint32_t cursorTextWidth = textLayout.width(text.substr(0, text.size() + terminal.getCursor() - terminal.getInput().size()));
    renderer.drawRect(layout.windowX + layout.marginLeft + cursorTextWidth, editorLayout.totalWindowHeight - layout.marginTop - editorLayout.lineHeight, 12, editorLayout.lineHeight, theme.terminalCursor);
}
#include <algorithm>

#include "TerminalView.h"
#include "Editor.h"

void TerminalView::render(RenderContext &renderContext, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties)
{
    if (editor.isTerminalVisible())
    {
        renderTerminal(renderContext, editor, textLayout, terminalLayout, windowProperties);
    }
}

void TerminalView::renderTerminal(RenderContext &renderContext, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties)
{
    const auto &theme = renderContext.getTheme();
    renderContext.drawRect(terminalLayout.viewport, theme.terminalBackground);
    const Terminal &terminal = editor.getTerminalConst();
    renderTerminalCursor(renderContext, terminal, textLayout, terminalLayout, windowProperties);
    renderTerminalSelection(renderContext, terminal, textLayout, terminalLayout, windowProperties);
    const std::string text = terminal.getPromptPrefix() + terminal.getInput();
    std::vector<std::string> output = terminal.getOutput().getText();
    std::reverse(output.begin(), output.end());

    // The TextBuffer starts with one empty line; skip empty lines in terminal output rendering.
    output.erase(std::remove_if(output.begin(), output.end(), [](const std::string &line)
                                { return line.empty(); }),
                 output.end());

    uint32_t visRows = terminal.getVisibleRows();
    uint32_t first = 0;
    uint32_t last = 0;

    if (!output.empty() && visRows > 0)
    {
        const uint32_t maxFirst = output.size() > visRows ? static_cast<uint32_t>(output.size()) - visRows : 0;
        first = std::min(terminal.getScrollOffset(), maxFirst);
        last = std::min(visRows, static_cast<uint32_t>(output.size()) - first);
    }

    for (uint32_t i = 0; i < last; ++i)
    {
        renderContext.drawText(output[first + i], terminalLayout.viewport.x + terminalLayout.marginLeft, windowProperties.totalWindowHeight - terminalLayout.marginTop - (i + 2) * windowProperties.lineHeight);
    }

    renderContext.drawText(text, terminalLayout.viewport.x + terminalLayout.marginLeft, windowProperties.totalWindowHeight - terminalLayout.marginTop - windowProperties.lineHeight);
}

void TerminalView::renderTerminalCursor(RenderContext &renderContext, const Terminal &terminal, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties)
{
    const auto &theme = renderContext.getTheme();
    const std::string text = terminal.getPromptPrefix() + terminal.getInput();
    uint32_t cursorTextWidth = textLayout.width(text.substr(0, text.size() + terminal.getCursor() - terminal.getInput().size()));
    renderContext.drawRect(terminalLayout.viewport.x + terminalLayout.marginLeft + cursorTextWidth, windowProperties.totalWindowHeight - terminalLayout.marginTop - windowProperties.lineHeight, 12, windowProperties.lineHeight, theme.terminalCursor);
}

void TerminalView::renderTerminalSelection(RenderContext &renderContext, const Terminal &terminal, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const Window_Properties &windowProperties)
{
    if (!terminal.hasSelection())
    {
        return;
    }

    const auto &theme = renderContext.getTheme();
    const std::string prefix = terminal.getPromptPrefix();
    const std::string &inputText = terminal.getInput();
    int inputBaseX = terminalLayout.viewport.x + terminalLayout.marginLeft + (int)textLayout.width(prefix);
    int inputY = windowProperties.totalWindowHeight - terminalLayout.marginTop - windowProperties.lineHeight;

    TextSelection sel = terminal.getSelection().normalized();
    int selStartX = inputBaseX + (int)textLayout.width(inputText.substr(0, sel.begin));
    int selWidth = (int)textLayout.width(inputText.substr(sel.begin, sel.end - sel.begin));
    renderContext.drawRect(selStartX, inputY, selWidth, windowProperties.lineHeight, theme.selection);
}
#include "TerminalView.h"
#include "Renderer.h"
#include "Editor.h"

void TerminalView::render(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const SDL_Properties &sdlProps)
{
    if (editor.isTerminalVisible())
    {
        renderTerminal(renderer, editor, textLayout, terminalLayout, sdlProps);
    }
}

void TerminalView::renderTerminal(Renderer &renderer, const Editor &editor, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const SDL_Properties &sdlProps)
{
    const auto &theme = renderer.getTheme();
    renderer.drawRect(terminalLayout.viewport, theme.terminalBackground);
    const Terminal &terminal = editor.getTerminalConst();
    renderTerminalCursor(renderer, terminal, textLayout, terminalLayout, sdlProps);
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
        renderer.drawText(output[first + i], terminalLayout.viewport.x + terminalLayout.marginLeft, sdlProps.totalWindowHeight - terminalLayout.marginTop - (i + 2) * sdlProps.lineHeight);
    }

    renderer.drawText(text, terminalLayout.viewport.x + terminalLayout.marginLeft, sdlProps.totalWindowHeight - terminalLayout.marginTop - sdlProps.lineHeight);
}

void TerminalView::renderTerminalCursor(Renderer &renderer, const Terminal &terminal, const TextLayout &textLayout, const TerminalLayout &terminalLayout, const SDL_Properties &sdlProps)
{
    const auto &theme = renderer.getTheme();
    const std::string &text = std::filesystem::current_path().string() + "$ " + terminal.getInput();
    uint32_t cursorTextWidth = textLayout.width(text.substr(0, text.size() + terminal.getCursor() - terminal.getInput().size()));
    renderer.drawRect(terminalLayout.viewport.x + terminalLayout.marginLeft + cursorTextWidth, sdlProps.totalWindowHeight - terminalLayout.marginTop - sdlProps.lineHeight, 12, sdlProps.lineHeight, theme.terminalCursor);
}
#include <gtest/gtest.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

#include <optional>
#include <string>
#include <vector>

#include "CommandProcessor.h"
#include "Terminal.h"
#include "types.h"

namespace
{
SDL_Event makeKeyDown(SDL_Keycode key, SDL_Keymod mod = SDL_KMOD_NONE)
{
    SDL_Event event{};
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.key = key;
    event.key.mod = mod;
    return event;
}

SDL_Event makeTextEvent(const char *text)
{
    SDL_Event event{};
    event.type = SDL_EVENT_TEXT_INPUT;
    event.text.text = text;
    return event;
}
} // namespace

TEST(CommandProcessorTests, UnknownCommandReturnsFailureAndWritesOutput)
{
    CommandProcessor processor;

    const CommandResult result = processor.executeCommand("does-not-exist", {});

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Unknown command");

    const auto &lines = processor.getOutput().getText();
    ASSERT_GE(lines.size(), 2u);
    EXPECT_EQ(lines.back(), "Unknown command");
}

TEST(CommandProcessorTests, QuitCommandCreatesPendingQuitRequest)
{
    CommandProcessor processor;

    const CommandResult result = processor.executeCommand("quit", {});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "Quit!");

    const std::optional<CommandRequest> pending = processor.consumeRequest();
    ASSERT_TRUE(pending.has_value());
    EXPECT_EQ(pending->type, CommandRequestType::Quit);
    EXPECT_EQ(pending->request, "");
    EXPECT_FALSE(processor.consumeRequest().has_value());
}

TEST(CommandProcessorTests, OpenWithoutArgsCreatesErrorRequest)
{
    CommandProcessor processor;

    const CommandResult result = processor.executeCommand("open", {});

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "No argument passed to open a file!");

    const std::optional<CommandRequest> pending = processor.consumeRequest();
    ASSERT_TRUE(pending.has_value());
    EXPECT_EQ(pending->type, CommandRequestType::Error);
    EXPECT_EQ(pending->request, "No argument passed to open file!");
}

TEST(CommandProcessorTests, ChangeLanguageCreatesRequest)
{
    CommandProcessor processor;

    const CommandResult result = processor.executeCommand("cl", {"cpp"});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "Changed language to cpp");

    const std::optional<CommandRequest> pending = processor.consumeRequest();
    ASSERT_TRUE(pending.has_value());
    EXPECT_EQ(pending->type, CommandRequestType::ChangeLanguage);
    EXPECT_EQ(pending->request, "cpp");
}

TEST(CommandProcessorTests, SaveCommandCreatesSaveRequest)
{
    CommandProcessor processor;

    const CommandResult result = processor.executeCommand("save", {});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "Saved current file!");

    const std::optional<CommandRequest> pending = processor.consumeRequest();
    ASSERT_TRUE(pending.has_value());
    EXPECT_EQ(pending->type, CommandRequestType::SaveFile);
    EXPECT_EQ(pending->request, "");
}

TEST(TerminalTests, ReturnParsesCommandAndCreatesOpenRequest)
{
    Terminal terminal;

    terminal.handleKey(makeTextEvent("open README.md"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    const std::optional<CommandRequest> request = terminal.consumeRequest();
    ASSERT_TRUE(request.has_value());
    EXPECT_EQ(request->type, CommandRequestType::OpenFile);
    EXPECT_EQ(request->request, "README.md");
    EXPECT_EQ(terminal.getInput(), "");
}

TEST(TerminalTests, UpDownNavigatesHistoryAndRestoresDraftInput)
{
    Terminal terminal;

    terminal.handleKey(makeTextEvent("save"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));
    terminal.handleKey(makeTextEvent("quit"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    terminal.handleKey(makeTextEvent("draft"));

    terminal.handleKey(makeKeyDown(SDLK_UP));
    EXPECT_EQ(terminal.getInput(), "quit");

    terminal.handleKey(makeKeyDown(SDLK_UP));
    EXPECT_EQ(terminal.getInput(), "save");

    terminal.handleKey(makeKeyDown(SDLK_DOWN));
    EXPECT_EQ(terminal.getInput(), "quit");

    terminal.handleKey(makeKeyDown(SDLK_DOWN));
    EXPECT_EQ(terminal.getInput(), "draft");
}

TEST(TerminalTests, CtrlUpAndCtrlDownScrollWithinBounds)
{
    Terminal terminal;
    terminal.setVisibleRows(1);

    terminal.handleKey(makeTextEvent("save"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));
    terminal.handleKey(makeTextEvent("save"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));
    terminal.handleKey(makeTextEvent("save"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    terminal.handleKey(makeKeyDown(SDLK_UP, SDL_KMOD_CTRL));
    terminal.handleKey(makeKeyDown(SDLK_UP, SDL_KMOD_CTRL));
    terminal.handleKey(makeKeyDown(SDLK_UP, SDL_KMOD_CTRL));
    EXPECT_EQ(terminal.getScrollOffset(), 2u);

    terminal.handleKey(makeKeyDown(SDLK_DOWN, SDL_KMOD_CTRL));
    EXPECT_EQ(terminal.getScrollOffset(), 1u);

    terminal.handleKey(makeKeyDown(SDLK_DOWN, SDL_KMOD_CTRL));
    terminal.handleKey(makeKeyDown(SDLK_DOWN, SDL_KMOD_CTRL));
    EXPECT_EQ(terminal.getScrollOffset(), 0u);
}

TEST(TerminalTests, EmptyReturnDoesNotCreateRequest)
{
    Terminal terminal;

    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    EXPECT_FALSE(terminal.consumeRequest().has_value());
    EXPECT_EQ(terminal.getOutput().getLineCount(), 1u);
}
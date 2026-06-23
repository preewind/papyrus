#include <gtest/gtest.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

#include <optional>
#include <string>
#include <vector>

#include "CommandProcessor.h"
#include "CommandRegistry.h"
#include "Terminal.h"
#include "TerminalActionRouter.h"
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

// ── CommandRegistry ──────────────────────────────────────────────────────────

TEST(CommandRegistryTests, FindReturnsNullptrForUnregisteredName)
{
    CommandRegistry reg;
    EXPECT_EQ(reg.find("unknown"), nullptr);
}

TEST(CommandRegistryTests, FindReturnsDefinitionAfterRegister)
{
    CommandRegistry reg;
    reg.registerCommand({"greet", "Say hello", "", [](const auto &) { return CommandResult{true, "hello"}; }, ""});

    const CommandDefinition *def = reg.find("greet");
    ASSERT_NE(def, nullptr);
    EXPECT_EQ(def->name, "greet");
}

TEST(CommandRegistryTests, RegisterOverwritesExistingCommandWithSameName)
{
    CommandRegistry reg;
    reg.registerCommand({"cmd", "v1", "", [](const auto &) { return CommandResult{true, "v1"}; }, ""});
    reg.registerCommand({"cmd", "v2", "", [](const auto &) { return CommandResult{true, "v2"}; }, ""});

    const CommandDefinition *def = reg.find("cmd");
    ASSERT_NE(def, nullptr);
    EXPECT_EQ(def->description, "v2");
}

TEST(CommandRegistryTests, BuildHelpTextContainsRegisteredNames)
{
    CommandRegistry reg;
    reg.registerCommand({"quit",  "Exit the editor",      "", [](const auto &) { return CommandResult{true, ""}; }, ""});
    reg.registerCommand({"open",  "Open a file",  "<file>", [](const auto &) { return CommandResult{true, ""}; }, ""});

    const std::string help = reg.buildHelpText();
    EXPECT_NE(help.find("quit"),  std::string::npos);
    EXPECT_NE(help.find("open"),  std::string::npos);
    EXPECT_NE(help.find("<file>"), std::string::npos);
    EXPECT_NE(help.find("help"),  std::string::npos); // built-in always present
    EXPECT_NE(help.find("!<cmd>"), std::string::npos);
}

// ── CommandProcessor ─────────────────────────────────────────────────────────

TEST(CommandProcessorTests, UnknownCommandReturnsFailure)
{
    CommandProcessor processor;

    const CommandResult result = processor.executeCommand("does-not-exist", {});

    EXPECT_FALSE(result.success);
    EXPECT_NE(result.message.find("Unknown command"), std::string::npos);
}

TEST(CommandProcessorTests, RegisteredCommandIsExecuted)
{
    CommandProcessor processor;
    bool called = false;

    processor.registerCommand({"ping", "Test command", "", [&called](const auto &)
    {
        called = true;
        return CommandResult{true, "pong"};
    }, ""});

    const CommandResult result = processor.executeCommand("ping", {});

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "pong");
    EXPECT_TRUE(called);
}

TEST(CommandProcessorTests, HelpCommandSucceeds)
{
    CommandProcessor processor;
    processor.registerCommand({"quit", "Exit", "", [](const auto &) { return CommandResult{true, ""}; }, ""});

    const CommandResult result = processor.executeCommand("help", {});
    EXPECT_TRUE(result.success);
}

TEST(CommandProcessorTests, OutputBufferContainsCommandResult)
{
    CommandProcessor processor;
    processor.registerCommand({"echo", "Echo args", "<msg>", [](const auto &args)
    {
        return CommandResult{true, args.empty() ? "" : args[0]};
    }, ""});

    processor.executeCommand("echo", {"hello"});

    const auto &lines = processor.getOutput().getText();
    const bool found = std::any_of(lines.begin(), lines.end(),
        [](const std::string &l) { return l == "hello"; });
    EXPECT_TRUE(found);
}

// ── Terminal ─────────────────────────────────────────────────────────────────

TEST(TerminalTests, ReturnCallsRegisteredCommandHandler)
{
    Terminal terminal;
    std::string capturedArg;

    terminal.registerCommand({"open", "Open file", "<file>", [&capturedArg](const std::vector<std::string> &args)
    {
        if (!args.empty()) capturedArg = args[0];
        return CommandResult{true, "ok"};
    }, ""});

    terminal.handleKey(makeTextEvent("open README.md"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    EXPECT_EQ(capturedArg, "README.md");
    EXPECT_EQ(terminal.getInput(), "");
}

TEST(TerminalTests, UpDownNavigatesHistoryAndRestoresDraftInput)
{
    Terminal terminal;
    terminal.registerCommand({"save", "Save", "", [](const auto &) { return CommandResult{true, ""}; }, ""});
    terminal.registerCommand({"quit", "Quit", "", [](const auto &) { return CommandResult{true, ""}; }, ""});

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
    terminal.registerCommand({"save", "Save", "", [](const auto &) { return CommandResult{true, "Saved current file!"}; }, ""});

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

TEST(TerminalTests, EmptyReturnDoesNotExecuteAnything)
{
    Terminal terminal;

    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    EXPECT_EQ(terminal.getOutput().getLineCount(), 1u);
}

TEST(TerminalTests, CtrlUpOnEmptyOutputKeepsScrollAtZero)
{
    Terminal terminal;
    terminal.setVisibleRows(8);

    terminal.handleKey(makeKeyDown(SDLK_UP, SDL_KMOD_CTRL));
    terminal.handleKey(makeKeyDown(SDLK_UP, SDL_KMOD_CTRL));

    EXPECT_EQ(terminal.getScrollOffset(), 0u);
}

// ── TerminalActionRouter ─────────────────────────────────────────────────────

TEST(TerminalActionRouterTests, DispatchesPendingActionResponse)
{
    Terminal terminal;
    TerminalActionRouter router;
    bool callbackCalled = false;
    std::string capturedInput;

    TerminalInputRequest request{
        .type = TerminalInputType::Filename,
        .prompt = "Enter filename to save: ",
        .defaultValue = "",
        .options = {}};

    const bool started = router.beginAction(
        terminal,
        "save_as_filename",
        request,
        [&](const TerminalInputResponse &response)
        {
            callbackCalled = true;
            capturedInput = response.userInput;
        });

    ASSERT_TRUE(started);
    ASSERT_TRUE(router.hasPendingAction());

    terminal.handleKey(makeTextEvent("scratch.txt"));
    terminal.handleKey(makeKeyDown(SDLK_RETURN));

    auto response = terminal.consumeInputResponse();
    ASSERT_TRUE(response.has_value());
    EXPECT_TRUE(router.dispatchResponse(*response));

    EXPECT_TRUE(callbackCalled);
    EXPECT_EQ(capturedInput, "scratch.txt");
    EXPECT_FALSE(router.hasPendingAction());
}
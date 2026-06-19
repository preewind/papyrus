#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "EditorCommandHandler.h"

namespace
{
struct FakeActionsState
{
    std::optional<std::filesystem::path> openPath;
    std::optional<Language> languageSet;
    std::optional<std::string> errorMessage;
    bool saveCalled = false;
    bool refreshCalled = false;
};

EditorCommandActions makeActions(FakeActionsState &state)
{
    return EditorCommandActions{
        .openFileRequest = [&state](const std::filesystem::path &path)
        {
            state.openPath = path;
        },
        .saveCurrentFile = [&state]()
        {
            state.saveCalled = true;
        },
        .setEditorLanguage = [&state](Language language)
        {
            state.languageSet = language;
        },
        .refreshEditorTokens = [&state]()
        {
            state.refreshCalled = true;
        },
        .reportEditorError = [&state](const std::string &message)
        {
            state.errorMessage = message;
        }};
}
} // namespace

TEST(EditorCommandHandlerTests, QuitReturnsPendingQuitRequest)
{
    EditorCommandHandler handler;
    FakeActionsState state;
    auto actions = makeActions(state);

    const auto pending = handler.handle({CommandRequestType::Quit, ""}, actions);

    ASSERT_TRUE(pending.has_value());
    EXPECT_EQ(pending->type, CommandRequestType::Quit);
    EXPECT_FALSE(state.saveCalled);
    EXPECT_FALSE(state.openPath.has_value());
}

TEST(EditorCommandHandlerTests, SaveFileInvokesSaveAction)
{
    EditorCommandHandler handler;
    FakeActionsState state;
    auto actions = makeActions(state);

    const auto pending = handler.handle({CommandRequestType::SaveFile, ""}, actions);

    EXPECT_FALSE(pending.has_value());
    EXPECT_TRUE(state.saveCalled);
}

TEST(EditorCommandHandlerTests, OpenFileInvokesOpenAction)
{
    EditorCommandHandler handler;
    FakeActionsState state;
    auto actions = makeActions(state);

    const auto pending = handler.handle({CommandRequestType::OpenFile, "notes.txt"}, actions);

    EXPECT_FALSE(pending.has_value());
    ASSERT_TRUE(state.openPath.has_value());
    EXPECT_EQ(state.openPath->string(), "notes.txt");
}

TEST(EditorCommandHandlerTests, ChangeLanguageToCppUpdatesLanguageAndRefreshes)
{
    EditorCommandHandler handler;
    FakeActionsState state;
    auto actions = makeActions(state);

    const auto pending = handler.handle({CommandRequestType::ChangeLanguage, "cpp"}, actions);

    EXPECT_FALSE(pending.has_value());
    ASSERT_TRUE(state.languageSet.has_value());
    EXPECT_EQ(*state.languageSet, Language::Cpp);
    EXPECT_TRUE(state.refreshCalled);
}

TEST(EditorCommandHandlerTests, ChangeLanguageUnknownOnlyRefreshesTokens)
{
    EditorCommandHandler handler;
    FakeActionsState state;
    auto actions = makeActions(state);

    const auto pending = handler.handle({CommandRequestType::ChangeLanguage, "python"}, actions);

    EXPECT_FALSE(pending.has_value());
    EXPECT_FALSE(state.languageSet.has_value());
    EXPECT_TRUE(state.refreshCalled);
}

TEST(EditorCommandHandlerTests, ErrorCommandReportsMessage)
{
    EditorCommandHandler handler;
    FakeActionsState state;
    auto actions = makeActions(state);

    const auto pending = handler.handle({CommandRequestType::Error, "bad command"}, actions);

    EXPECT_FALSE(pending.has_value());
    ASSERT_TRUE(state.errorMessage.has_value());
    EXPECT_EQ(*state.errorMessage, "bad command");
}

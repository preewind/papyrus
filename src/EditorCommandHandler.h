#pragma once

#include <optional>
#include <filesystem>
#include <string>
#include <functional>

#include "types.h"

struct EditorCommandActions
{
    std::function<void(const std::filesystem::path &path)> openFileRequest;
    std::function<void()> saveCurrentFile;
    std::function<void(Language language)> setEditorLanguage;
    std::function<void()> refreshEditorTokens;
    std::function<void(const std::string &message)> reportEditorError;
};

class EditorCommandHandler
{
public:
    std::optional<CommandRequest> handle(const CommandRequest &request, EditorCommandActions &actions) const;
};

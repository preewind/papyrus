#include "EditorCommandHandler.h"

std::optional<CommandRequest> EditorCommandHandler::handle(const CommandRequest &request, const EditorCommandActions &actions) const
{
    switch (request.type)
    {
    case CommandRequestType::Quit:
        return CommandRequest{CommandRequestType::Quit, ""};
    case CommandRequestType::SaveFile:
        actions.saveCurrentFile();
        return std::nullopt;
    case CommandRequestType::ChangeLanguage:
        if (request.request == "cpp")
        {
            actions.setEditorLanguage(Language::Cpp);
            actions.refreshEditorTokens();
        }
        return std::nullopt;
    case CommandRequestType::OpenFile:
        actions.openFileRequest(request.request);
        return std::nullopt;
    case CommandRequestType::Error:
        actions.reportEditorError(request.request);
        return std::nullopt;
    default:
        actions.reportEditorError("Invalid command!");
        return std::nullopt;
    }
}

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <utility>

#include "Terminal.h"

class TerminalActionRouter
{
public:
    using ActionCallback = std::function<void(const TerminalInputResponse &response)>;

    bool beginAction(Terminal &terminal, std::string actionKey, const TerminalInputRequest &request, ActionCallback callback)
    {
        if (mPendingAction.has_value())
        {
            return false;
        }

        mPendingAction = PendingAction{
            .actionKey = std::move(actionKey),
            .callback = std::move(callback)};
        terminal.requestInput(request);
        return true;
    }

    bool hasPendingAction() const
    {
        return mPendingAction.has_value();
    }

    std::optional<std::string> getPendingActionKey() const
    {
        if (!mPendingAction)
        {
            return std::nullopt;
        }
        return mPendingAction->actionKey;
    }

    bool dispatchResponse(const TerminalInputResponse &response)
    {
        if (!mPendingAction)
        {
            return false;
        }

        PendingAction pending = std::move(*mPendingAction);
        mPendingAction.reset();
        pending.callback(response);
        return true;
    }

    void clear()
    {
        mPendingAction.reset();
    }

private:
    struct PendingAction
    {
        std::string actionKey;
        ActionCallback callback;
    };

    std::optional<PendingAction> mPendingAction;
};

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct ArgParseResult
{
    std::string executableName;
    std::unordered_map<std::string, std::optional<std::string>> flags;
    std::vector<std::string> positional;

    bool hasFlag(std::string_view name) const
    {
        return flags.contains(std::string(name));
    }

    std::optional<std::string> getFlagValue(std::string_view name) const
    {
        const auto it = flags.find(std::string(name));
        if (it == flags.end())
        {
            return std::nullopt;
        }
        return it->second;
    }

    std::optional<std::string> firstPositional() const
    {
        if (positional.empty())
        {
            return std::nullopt;
        }
        return positional.front();
    }
};

class ArgParser
{
public:
    ArgParseResult parse(int argc, char *argv[]) const
    {
        ArgParseResult result;

        if (argc > 0 && argv[0])
        {
            result.executableName = argv[0];
        }

        bool parseAsPositional = false;
        for (int i = 1; i < argc; ++i)
        {
            if (!argv[i])
            {
                continue;
            }

            const std::string token = argv[i];

            if (parseAsPositional || token == "-")
            {
                result.positional.push_back(token);
                continue;
            }

            if (token == "--")
            {
                parseAsPositional = true;
                continue;
            }

            if (token.starts_with("--"))
            {
                const std::string raw = token.substr(2);
                const size_t equalPos = raw.find('=');
                if (equalPos != std::string::npos)
                {
                    const std::string name = raw.substr(0, equalPos);
                    const std::string value = raw.substr(equalPos + 1);
                    result.flags[name] = value;
                    continue;
                }

                if (i + 1 < argc)
                {
                    const std::string next = argv[i + 1];
                    if (!next.empty() && next[0] != '-')
                    {
                        result.flags[raw] = next;
                        ++i;
                        continue;
                    }
                }

                result.flags[raw] = std::nullopt;
                continue;
            }

            if (token.starts_with("-") && token.size() > 1)
            {
                const std::string raw = token.substr(1);

                // support short flag bundles like -abc
                if (raw.size() > 1)
                {
                    for (char c : raw)
                    {
                        result.flags[std::string(1, c)] = std::nullopt;
                    }
                    continue;
                }

                if (i + 1 < argc)
                {
                    const std::string next = argv[i + 1];
                    if (!next.empty() && next[0] != '-')
                    {
                        result.flags[raw] = next;
                        ++i;
                        continue;
                    }
                }

                result.flags[raw] = std::nullopt;
                continue;
            }

            result.positional.push_back(token);
        }

        return result;
    }
};
#include <iostream>
#include <string>

#include "logger.h"

enum ArgumentType
{
    POSITIONAL = 0,
    BOOL_FLAG,
    VALUE_FLAG
};

struct Argument
{
    ArgumentType type;
    std::string name;
    std::string value = "";

    Argument(ArgumentType t, std::string n, std::string v = "")
        : type(t), name(n), value(v) {}

    bool operator==(const Argument &other) const
    {
        return (type == other.type) && (name == other.name);
    }
    bool operator!=(const Argument &other) const
    {
        return !(*this == other);
    }

    friend std::ostream &operator<<(std::ostream &os, const Argument &arg)
    {
        std::string typeStr = "unknown";
        switch (arg.type)
        {
        case ArgumentType::BOOL_FLAG:
            typeStr = "bool flag";
            break;
        case ArgumentType::VALUE_FLAG:
            typeStr = "value flag";
            break;
        case ArgumentType::POSITIONAL:
            typeStr = "positional";
            break;
        }
        os << "type: " << typeStr << ", name: " << arg.name << ", Value: " << arg.value;
        return os;
    }
};

class ArgParser
{

public:
    ArgParser() = default;

    bool hasArgument(const Argument &arg)
    {
        for (const Argument &a : mArgs)
        {
            if (arg == a)
            {
                return true;
            }
        }
        return false;
    }

    // arg passed here is the argument you want to have parsed and get its value
    const std::string getArgumentValue(const Argument &arg) const
    {

        for (const Argument &a : mArgs)
        {
            if (arg == a)
            {
                return a.value;
            }
        }
        return "";
    }

    void parse(int argc, char *argv[])
    {

        if (argc > 0)
        {
            mExecutableName = argv[0];
        }

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg.starts_with("-"))
            {
                // handle single dash and double dash the same for now
                bool isLong = arg.starts_with("--");
                std::string name = arg.substr(isLong ? 2 : 1);

                // Check if it has a trailing value
                if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-"))
                {
                    mArgs.push_back(Argument(ArgumentType::VALUE_FLAG, name, argv[i + 1]));
                    i++;
                }
                else
                {
                    mArgs.push_back(Argument(ArgumentType::BOOL_FLAG, name));
                }
            }
            else
            {
                mArgs.push_back(Argument(ArgumentType::POSITIONAL, "positional", arg));
            }
        }
    }

private:
    std::string mExecutableName;
    std::vector<Argument> mArgs;
};
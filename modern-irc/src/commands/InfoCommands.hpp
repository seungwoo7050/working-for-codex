#ifndef INFOCOMMANDS_HPP
#define INFOCOMMANDS_HPP

#include "Command.hpp"

// WHOIS command - get information about a user
class WhoisCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "WHOIS"; }
};

// WHO command - list users matching a pattern
class WhoCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "WHO"; }
};

#endif

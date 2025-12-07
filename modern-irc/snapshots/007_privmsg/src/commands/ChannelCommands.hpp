#ifndef CHANNELCOMMANDS_HPP
#define CHANNELCOMMANDS_HPP

#include "Command.hpp"

class JoinCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "JOIN"; }
};

class PartCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PART"; }
};

class NamesCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "NAMES"; }
};

class ListCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "LIST"; }
};

#endif

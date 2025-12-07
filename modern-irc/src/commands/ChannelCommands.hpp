#ifndef CHANNELCOMMANDS_HPP
#define CHANNELCOMMANDS_HPP

#include "Command.hpp"

// JOIN command - join a channel
class JoinCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "JOIN"; }
};

// PART command - leave a channel
class PartCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PART"; }
};

// NAMES command - list users in a channel
class NamesCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "NAMES"; }
};

// LIST command - list all channels
class ListCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "LIST"; }
};

#endif

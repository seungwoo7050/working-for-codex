#ifndef MESSAGECOMMANDS_HPP
#define MESSAGECOMMANDS_HPP

#include "Command.hpp"

// PRIVMSG command - send message to user or channel
class PrivmsgCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PRIVMSG"; }
};

// NOTICE command - send notice to user or channel
class NoticeCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "NOTICE"; }
};

// PING command - keep alive
class PingCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PING"; }
};

// PONG command - response to PING
class PongCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PONG"; }
};

#endif

#ifndef MESSAGECOMMANDS_HPP
#define MESSAGECOMMANDS_HPP

#include "Command.hpp"

class PrivmsgCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PRIVMSG"; }
};

class NoticeCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "NOTICE"; }
};

class PingCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PING"; }
};

class PongCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PONG"; }
};

#endif

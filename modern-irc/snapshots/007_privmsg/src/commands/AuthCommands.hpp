#ifndef AUTHCOMMANDS_HPP
#define AUTHCOMMANDS_HPP

#include "Command.hpp"

class PassCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PASS"; }
};

class NickCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "NICK"; }
};

class UserCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "USER"; }
};

class QuitCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "QUIT"; }
};

#endif

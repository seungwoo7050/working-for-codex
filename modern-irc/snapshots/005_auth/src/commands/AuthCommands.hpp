#ifndef AUTHCOMMANDS_HPP
#define AUTHCOMMANDS_HPP

#include "Command.hpp"

// PASS command - authenticate with server password
class PassCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "PASS"; }
};

// NICK command - set nickname
class NickCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "NICK"; }
};

// USER command - set username and realname
class UserCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "USER"; }
};

// QUIT command - disconnect from server
class QuitCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "QUIT"; }
};

#endif

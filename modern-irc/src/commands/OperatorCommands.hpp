#ifndef OPERATORCOMMANDS_HPP
#define OPERATORCOMMANDS_HPP

#include "Command.hpp"

class Channel;

// KICK command - remove user from channel (operators only)
class KickCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "KICK"; }
};

// INVITE command - invite user to channel
class InviteCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "INVITE"; }
};

// TOPIC command - view or set channel topic
class TopicCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "TOPIC"; }
};

// MODE command - set channel or user modes
class ModeCommand : public Command {
public:
    void execute(Server& server, Client& client, const Message& message);
    std::string getName() const { return "MODE"; }
private:
    void handleChannelMode(Server& server, Client& client, const Message& message, Channel* channel);
    void handleUserMode(Server& server, Client& client, const Message& message);
};

#endif

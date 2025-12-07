#ifndef SERVERSTATSCOMMANDS_HPP
#define SERVERSTATSCOMMANDS_HPP

#include "Command.hpp"

// STATS - Server statistics
class StatsCommand : public Command {
public:
    virtual ~StatsCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "STATS"; }
};

// INFO - Server information
class InfoCommand : public Command {
public:
    virtual ~InfoCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "INFO"; }
};

// VERSION - Server version
class VersionCommand : public Command {
public:
    virtual ~VersionCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "VERSION"; }
};

// TIME - Server time
class TimeCommand : public Command {
public:
    virtual ~TimeCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "TIME"; }
};

// LUSERS - User statistics
class LusersCommand : public Command {
public:
    virtual ~LusersCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "LUSERS"; }
};

#endif

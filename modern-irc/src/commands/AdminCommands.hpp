#ifndef ADMINCOMMANDS_HPP
#define ADMINCOMMANDS_HPP

#include "Command.hpp"

// OPER - Gain operator privileges
class OperCommand : public Command {
public:
    virtual ~OperCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "OPER"; }
};

// KILL - Forcibly disconnect a client (operator only)
class KillCommand : public Command {
public:
    virtual ~KillCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "KILL"; }
};

// REHASH - Reload server configuration (operator only)
class RehashCommand : public Command {
public:
    virtual ~RehashCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "REHASH"; }
};

// RESTART - Restart the server (operator only)
class RestartCommand : public Command {
public:
    virtual ~RestartCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "RESTART"; }
};

// DIE - Shutdown the server (operator only)
class DieCommand : public Command {
public:
    virtual ~DieCommand() {}
    virtual void execute(Server& server, Client& client, const Message& msg);
    virtual std::string getName() const { return "DIE"; }
};

#endif

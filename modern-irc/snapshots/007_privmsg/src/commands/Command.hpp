#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class Server;
class Client;
class Message;

class Command {
public:
    virtual ~Command() {}
    virtual void execute(Server& server, Client& client, const Message& message) = 0;
    virtual std::string getName() const = 0;
};

#endif

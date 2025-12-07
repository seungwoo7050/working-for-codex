#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <memory>
#include "../network/Socket.hpp"
#include "../network/Poller.hpp"
#include "Client.hpp"
#include "Message.hpp"

class Command;

// IRC numeric replies (RPL_*)
#define RPL_WELCOME 001

// IRC error codes (ERR_*)
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NOTONCHANNEL 442
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464

class Server {
private:
    std::string serverName;
    std::string password;
    int port;
    Socket serverSocket;
    Poller poller;
    std::map<int, std::unique_ptr<Client>> clients;
    std::map<std::string, std::unique_ptr<Command>> commands;
    bool running;

    void acceptNewClient();
    void handleClientData(int clientFd);
    void handleClientDisconnect(int clientFd);
    void processMessage(Client& client, const Message& message);
    void registerCommands();

public:
    Server(int port, const std::string& password);
    ~Server();

    bool initialize();
    void run();
    void stop();

    Client* getClient(int fd);
    Client* getClientByNick(const std::string& nickname);
    void removeClient(int fd);
    void sendToClient(Client& client, const std::string& message);
    void sendToClient(int fd, const std::string& message);

    // Message broadcasting (placeholder for channels)
    void broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd = -1);

    std::string getServerName() const;
    std::string getPassword() const;
    bool isNicknameInUse(const std::string& nickname) const;
    std::string formatReply(int code, const Client& client, const std::string& message) const;
    std::string formatReply(int code, const Client& client, const std::string& param, const std::string& message) const;
};

#endif

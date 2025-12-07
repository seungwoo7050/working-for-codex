#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <memory>
#include "../network/Socket.hpp"
#include "../network/Poller.hpp"
#include "Client.hpp"

class Server {
private:
    std::string serverName;
    std::string password;
    int port;
    Socket serverSocket;
    Poller poller;
    std::map<int, std::unique_ptr<Client>> clients;
    bool running;

    void acceptNewClient();
    void handleClientData(int clientFd);
    void handleClientDisconnect(int clientFd);

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

    std::string getServerName() const;
    std::string getPassword() const;
    bool isNicknameInUse(const std::string& nickname) const;
};

#endif

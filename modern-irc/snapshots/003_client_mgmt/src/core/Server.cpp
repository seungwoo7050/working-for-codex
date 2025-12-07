#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>

Server::Server(int p, const std::string& pass)
    : serverName("irc.local"), password(pass), port(p), running(false) {
}

Server::~Server() {
    stop();
}

bool Server::initialize() {
    if (!serverSocket.create()) return false;
    if (!serverSocket.setReuseAddr()) return false;
    if (!serverSocket.setNonBlocking()) return false;
    if (!serverSocket.bind(port)) return false;
    if (!serverSocket.listen()) return false;

    if (!poller.addFd(serverSocket.getFd(), POLLIN)) {
        std::cerr << "Failed to add server socket to poller" << std::endl;
        return false;
    }

    std::cout << "Server listening on port " << port << std::endl;
    return true;
}

void Server::run() {
    running = true;

    while (running) {
        int ret = poller.poll(1000);

        if (ret < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ret == 0) continue;

        auto fds = poller.getFds();
        for (int fd : fds) {
            if (poller.hasEvent(fd, POLLERR) || poller.hasEvent(fd, POLLHUP)) {
                if (fd != serverSocket.getFd()) {
                    handleClientDisconnect(fd);
                }
                continue;
            }

            if (poller.hasEvent(fd, POLLIN)) {
                if (fd == serverSocket.getFd()) {
                    acceptNewClient();
                } else {
                    handleClientData(fd);
                }
            }

            if (poller.hasEvent(fd, POLLOUT)) {
                auto* client = getClient(fd);
                if (client && !client->getSendBuffer().empty()) {
                    const auto& buffer = client->getSendBuffer();
                    ssize_t sent = send(fd, buffer.c_str(), buffer.length(), MSG_NOSIGNAL);
                    if (sent > 0) {
                        client->clearSendBuffer(sent);
                    }
                }
            }
        }
    }
}

void Server::stop() {
    running = false;
}

void Server::acceptNewClient() {
    int clientFd = serverSocket.accept();
    if (clientFd < 0) return;

    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
    }

    clients[clientFd] = std::make_unique<Client>(clientFd);
    poller.addFd(clientFd, POLLIN | POLLOUT);

    std::cout << "New client connected: fd=" << clientFd << std::endl;
}

void Server::handleClientData(int clientFd) {
    auto* client = getClient(clientFd);
    if (!client) return;

    char buffer[512];
    ssize_t received = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (received <= 0) {
        if (received == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
            handleClientDisconnect(clientFd);
        }
        return;
    }

    buffer[received] = '\0';
    client->appendToRecvBuffer(std::string(buffer, received));

    // Echo back for now (commands will be in later snapshots)
    while (client->hasCompleteMessage()) {
        std::string msg = client->extractMessage();
        std::cout << "Received from fd=" << clientFd << ": " << msg << std::endl;
        // Echo
        sendToClient(*client, msg + "\r\n");
    }
}

void Server::handleClientDisconnect(int clientFd) {
    auto* client = getClient(clientFd);
    if (client) {
        std::cout << "Client disconnected: fd=" << clientFd;
        if (!client->getNickname().empty()) {
            std::cout << " (" << client->getNickname() << ")";
        }
        std::cout << std::endl;
    }
    removeClient(clientFd);
}

Client* Server::getClient(int fd) {
    auto it = clients.find(fd);
    if (it != clients.end()) {
        return it->second.get();
    }
    return nullptr;
}

Client* Server::getClientByNick(const std::string& nickname) {
    for (const auto& [fd, client] : clients) {
        if (client->getNickname() == nickname) {
            return client.get();
        }
    }
    return nullptr;
}

void Server::removeClient(int fd) {
    poller.removeFd(fd);
    close(fd);
    clients.erase(fd);
}

void Server::sendToClient(Client& client, const std::string& message) {
    client.appendToSendBuffer(message);
}

void Server::sendToClient(int fd, const std::string& message) {
    auto* client = getClient(fd);
    if (client) {
        sendToClient(*client, message);
    }
}

std::string Server::getServerName() const {
    return serverName;
}

std::string Server::getPassword() const {
    return password;
}

bool Server::isNicknameInUse(const std::string& nickname) const {
    for (const auto& [fd, client] : clients) {
        if (client->getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

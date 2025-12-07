#include "Server.hpp"
#include "../commands/AuthCommands.hpp"
#include "../commands/ChannelCommands.hpp"
#include "../commands/MessageCommands.hpp"
#include "../utils/StringUtils.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

Server::Server(int p, const std::string& pass)
    : serverName("irc.local"), password(pass), port(p), running(false) {
}

Server::~Server() {
    stop();
}

void Server::registerCommands() {
    // Auth commands
    commands["PASS"] = std::make_unique<PassCommand>();
    commands["NICK"] = std::make_unique<NickCommand>();
    commands["USER"] = std::make_unique<UserCommand>();
    commands["QUIT"] = std::make_unique<QuitCommand>();

    // Channel commands
    commands["JOIN"] = std::make_unique<JoinCommand>();
    commands["PART"] = std::make_unique<PartCommand>();
    commands["NAMES"] = std::make_unique<NamesCommand>();
    commands["LIST"] = std::make_unique<ListCommand>();

    // Message commands
    commands["PRIVMSG"] = std::make_unique<PrivmsgCommand>();
    commands["NOTICE"] = std::make_unique<NoticeCommand>();
    commands["PING"] = std::make_unique<PingCommand>();
    commands["PONG"] = std::make_unique<PongCommand>();
}

bool Server::initialize() {
    registerCommands();

    if (!serverSocket.create()) return false;
    if (!serverSocket.setReuseAddr()) return false;
    if (!serverSocket.setNonBlocking()) return false;
    if (!serverSocket.bind(port)) return false;
    if (!serverSocket.listen()) return false;

    if (!poller.addFd(serverSocket.getFd(), POLLIN)) {
        std::cerr << "Failed to add server socket to poller" << std::endl;
        return false;
    }

    std::cout << "modern-irc v1.0.0 listening on port " << port << std::endl;
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

    while (client->hasCompleteMessage()) {
        std::string rawMessage = client->extractMessage();
        if (rawMessage.empty()) continue;

        Message message;
        if (message.parse(rawMessage)) {
            processMessage(*client, message);
        }
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

        const auto& channelList = client->getChannels();
        auto channelsCopy = channelList;
        for (const auto& channelName : channelsCopy) {
            auto* channel = getChannel(channelName);
            if (channel) {
                channel->removeMember(clientFd);
                if (channel->isEmpty()) {
                    removeChannel(channelName);
                }
            }
        }
    }
    removeClient(clientFd);
}

void Server::processMessage(Client& client, const Message& message) {
    auto cmd = StringUtils::toUpper(message.getCommand());

    if (auto it = commands.find(cmd); it != commands.end()) {
        it->second->execute(*this, client, message);
    }
}

Client* Server::getClient(int fd) {
    if (auto it = clients.find(fd); it != clients.end()) {
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

Channel* Server::getChannel(const std::string& name) {
    if (auto it = channels.find(name); it != channels.end()) {
        return it->second.get();
    }
    return nullptr;
}

Channel* Server::createChannel(const std::string& name) {
    auto channel = std::make_unique<Channel>(name);
    auto* ptr = channel.get();
    channels[name] = std::move(channel);
    return ptr;
}

void Server::removeChannel(const std::string& name) {
    channels.erase(name);
}

std::vector<std::string> Server::getChannelList() const {
    std::vector<std::string> result;
    for (const auto& [name, channel] : channels) {
        result.push_back(name);
    }
    return result;
}

void Server::broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd) {
    auto* channel = getChannel(channelName);
    if (!channel) return;

    const auto& members = channel->getMembers();
    for (int memberFd : members) {
        if (memberFd != excludeFd) {
            sendToClient(memberFd, message);
        }
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

std::string Server::formatReply(int code, const Client& client, const std::string& message) const {
    std::ostringstream oss;
    oss << ":" << serverName << " ";
    if (code < 100) oss << "0";
    if (code < 10) oss << "0";
    oss << code << " ";
    std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
    oss << nick << " :" << message << "\r\n";
    return oss.str();
}

std::string Server::formatReply(int code, const Client& client, const std::string& param, const std::string& message) const {
    std::ostringstream oss;
    oss << ":" << serverName << " ";
    if (code < 100) oss << "0";
    if (code < 10) oss << "0";
    oss << code << " ";
    std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
    oss << nick << " " << param << " :" << message << "\r\n";
    return oss.str();
}

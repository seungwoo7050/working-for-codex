#include "Server.hpp"
#include "../commands/AuthCommands.hpp"
#include "../commands/ChannelCommands.hpp"
#include "../commands/MessageCommands.hpp"
#include "../commands/OperatorCommands.hpp"
#include "../commands/InfoCommands.hpp"
#include "../commands/AdminCommands.hpp"
#include "../commands/ServerStatsCommands.hpp"
#include "../utils/StringUtils.hpp"
#include "../utils/TopicPersistence.hpp"
#include "../utils/Logger.hpp"
#include "../utils/Config.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cerrno>
#include <cstring>

Server::Server(int p, const std::string& pass)
    : serverName("irc.local"), password(pass), operPassword("operpass"), port(p), running(false),
      startTime(time(nullptr)), totalConnections(0), totalMessages(0), peakClientCount(0), totalChannelsCreated(0) {
}

Server::~Server() {
    stop();
    // Smart pointers automatically clean up clients, channels, and commands
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

    // Operator commands (Phase 2)
    commands["KICK"] = std::make_unique<KickCommand>();
    commands["INVITE"] = std::make_unique<InviteCommand>();
    commands["TOPIC"] = std::make_unique<TopicCommand>();
    commands["MODE"] = std::make_unique<ModeCommand>();

    // Info commands (Phase 2)
    commands["WHOIS"] = std::make_unique<WhoisCommand>();
    commands["WHO"] = std::make_unique<WhoCommand>();

    // Admin commands (Phase 3)
    commands["OPER"] = std::make_unique<OperCommand>();
    commands["KILL"] = std::make_unique<KillCommand>();
    commands["REHASH"] = std::make_unique<RehashCommand>();
    commands["RESTART"] = std::make_unique<RestartCommand>();
    commands["DIE"] = std::make_unique<DieCommand>();

    // Server stats commands (Phase 3)
    commands["STATS"] = std::make_unique<StatsCommand>();
    commands["INFO"] = std::make_unique<InfoCommand>();
    commands["VERSION"] = std::make_unique<VersionCommand>();
    commands["TIME"] = std::make_unique<TimeCommand>();
    commands["LUSERS"] = std::make_unique<LusersCommand>();
}

bool Server::initialize() {
    registerCommands();
    loadChannelTopics();

    // Create server socket
    if (!serverSocket.create()) {
        return false;
    }

    // Set socket options
    if (!serverSocket.setReuseAddr()) {
        return false;
    }

    if (!serverSocket.setNonBlocking()) {
        return false;
    }

    // Bind to port
    if (!serverSocket.bind(port)) {
        return false;
    }

    // Listen for connections
    if (!serverSocket.listen()) {
        return false;
    }

    // Add server socket to poller
    if (!poller.addFd(serverSocket.getFd(), POLLIN)) {
        std::cerr << "Failed to add server socket to poller" << std::endl;
        return false;
    }

    std::cout << "modern-irc listening on port " << port << std::endl;
    return true;
}

void Server::run() {
    running = true;

    while (running) {
        int ret = poller.poll(1000);  // 1 second timeout

        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "Poll error" << std::endl;
            break;
        }

        if (ret == 0) {
            continue;  // Timeout, no events
        }

        // Get all file descriptors
        auto fds = poller.getFds();

        for (int fd : fds) {
            // Check for errors
            if (poller.hasEvent(fd, POLLERR) || poller.hasEvent(fd, POLLHUP)) {
                if (fd == serverSocket.getFd()) {
                    std::cerr << "Server socket error" << std::endl;
                    running = false;
                    break;
                } else {
                    handleClientDisconnect(fd);
                }
                continue;
            }

            // Check for incoming data
            if (poller.hasEvent(fd, POLLIN)) {
                if (fd == serverSocket.getFd()) {
                    acceptNewClient();
                } else {
                    handleClientData(fd);
                }
            }

            // Check for outgoing data
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
    if (clientFd < 0) {
        return;
    }

    // Set client socket to non-blocking
    int flags = fcntl(clientFd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
    }

    // Create client object
    clients[clientFd] = std::make_unique<Client>(clientFd);
    totalConnections++;
    updatePeakClientCount();

    // Add to poller
    poller.addFd(clientFd, POLLIN | POLLOUT);

    std::cout << "New client connected: fd=" << clientFd << std::endl;
}

void Server::handleClientData(int clientFd) {
    auto* client = getClient(clientFd);
    if (!client) {
        return;
    }

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

    // Process all complete messages
    while (client->hasCompleteMessage()) {
        std::string rawMessage = client->extractMessage();
        if (rawMessage.empty()) {
            continue;
        }

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

        // Remove from all channels
        const auto& channelList = client->getChannels();
        auto channelsCopy = channelList;  // Copy to avoid iterator invalidation
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
    clients.erase(fd);  // unique_ptr automatically deletes the Client
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
    totalChannelsCreated++;
    return ptr;
}

void Server::removeChannel(const std::string& name) {
    channels.erase(name);  // unique_ptr automatically deletes the Channel
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
    if (!channel) {
        return;
    }

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

    // Format code as 3 digits
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

    // Format code as 3 digits
    if (code < 100) oss << "0";
    if (code < 10) oss << "0";
    oss << code << " ";

    std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
    oss << nick << " " << param << " :" << message << "\r\n";

    return oss.str();
}

void Server::loadChannelTopics() {
    // This will be called when channels are created
    // For now, it's a placeholder since channels are created dynamically
}

void Server::saveChannelTopic(const std::string& channelName, const std::string& topic) {
    TopicPersistence::saveTopic(channelName, topic);
}

// Phase 3: Statistics methods
std::string Server::getOperPassword() const {
    return operPassword;
}

void Server::setOperPassword(const std::string& pass) {
    operPassword = pass;
}

void Server::incrementMessageCount() {
    totalMessages++;
}

void Server::updatePeakClientCount() {
    if (clients.size() > peakClientCount) {
        peakClientCount = clients.size();
    }
}

time_t Server::getStartTime() const {
    return startTime;
}

size_t Server::getTotalConnections() const {
    return totalConnections;
}

size_t Server::getTotalMessages() const {
    return totalMessages;
}

size_t Server::getPeakClientCount() const {
    return peakClientCount;
}

size_t Server::getTotalChannelsCreated() const {
    return totalChannelsCreated;
}

size_t Server::getCurrentClientCount() const {
    return clients.size();
}

size_t Server::getCurrentChannelCount() const {
    return channels.size();
}

std::map<int, std::unique_ptr<Client>>& Server::getClients() {
    return clients;
}

bool Server::reloadConfiguration(const std::string& configPath) {
    // This would reload the configuration file
    // For now, it's a placeholder that returns success
    std::cout << "Reloading configuration from: " << configPath << std::endl;
    return true;
}

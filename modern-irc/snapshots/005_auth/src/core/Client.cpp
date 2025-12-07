#include "Client.hpp"
#include <sstream>

Client::Client(int clientFd)
    : fd(clientFd),
      hostname("localhost"),
      authenticated(false),
      registered(false) {
}

Client::~Client() {
}

int Client::getFd() const {
    return fd;
}

const std::string& Client::getNickname() const {
    return nickname;
}

const std::string& Client::getUsername() const {
    return username;
}

const std::string& Client::getRealname() const {
    return realname;
}

const std::string& Client::getHostname() const {
    return hostname;
}

const std::set<std::string>& Client::getChannels() const {
    return channels;
}

bool Client::isAuthenticated() const {
    return authenticated;
}

bool Client::isRegistered() const {
    return registered;
}

void Client::setNickname(const std::string& nick) {
    nickname = nick;
}

void Client::setUsername(const std::string& user) {
    username = user;
}

void Client::setRealname(const std::string& real) {
    realname = real;
}

void Client::setHostname(const std::string& host) {
    hostname = host;
}

void Client::setAuthenticated(bool auth) {
    authenticated = auth;
}

void Client::setRegistered(bool reg) {
    registered = reg;
}

void Client::joinChannel(const std::string& channel) {
    channels.insert(channel);
}

void Client::leaveChannel(const std::string& channel) {
    channels.erase(channel);
}

bool Client::isInChannel(const std::string& channel) const {
    return channels.find(channel) != channels.end();
}

void Client::appendToRecvBuffer(const std::string& data) {
    recvBuffer += data;
}

std::string Client::extractMessage() {
    size_t pos = recvBuffer.find("\r\n");
    if (pos == std::string::npos) {
        pos = recvBuffer.find("\n");
        if (pos == std::string::npos) {
            return "";
        }
    }

    std::string message = recvBuffer.substr(0, pos);
    recvBuffer.erase(0, pos + (recvBuffer[pos] == '\r' ? 2 : 1));
    return message;
}

bool Client::hasCompleteMessage() const {
    return recvBuffer.find("\r\n") != std::string::npos ||
           recvBuffer.find("\n") != std::string::npos;
}

void Client::appendToSendBuffer(const std::string& data) {
    sendBuffer += data;
}

const std::string& Client::getSendBuffer() const {
    return sendBuffer;
}

void Client::clearSendBuffer(size_t bytes) {
    if (bytes >= sendBuffer.length()) {
        sendBuffer.clear();
    } else {
        sendBuffer.erase(0, bytes);
    }
}

std::string Client::getPrefix() const {
    std::ostringstream oss;
    oss << nickname;
    if (!username.empty()) {
        oss << "!" << username;
    }
    if (!hostname.empty()) {
        oss << "@" << hostname;
    }
    return oss.str();
}

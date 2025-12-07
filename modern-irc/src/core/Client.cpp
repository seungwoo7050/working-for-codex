#include "Client.hpp"
#include <sstream>

Client::Client(int clientFd)
    : fd(clientFd),
      hostname("localhost"),
      authenticated(false),
      registered(false),
      isOperator(false),
      connectionTime(time(nullptr)),
      totalMessageCount(0) {
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

bool Client::isOper() const {
    return isOperator;
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

void Client::setOperator(bool oper) {
    isOperator = oper;
    // Update +o mode accordingly
    if (oper) {
        userModes['o'] = true;
    } else {
        userModes.erase('o');
    }
}

void Client::setUserMode(char mode, bool value) {
    if (value) {
        userModes[mode] = true;
    } else {
        userModes.erase(mode);
    }
}

bool Client::hasUserMode(char mode) const {
    std::map<char, bool>::const_iterator it = userModes.find(mode);
    return it != userModes.end() && it->second;
}

std::string Client::getUserModes() const {
    std::string modes = "+";
    for (std::map<char, bool>::const_iterator it = userModes.begin(); it != userModes.end(); ++it) {
        if (it->second) {
            modes += it->first;
        }
    }
    return modes.length() > 1 ? modes : "";
}

void Client::setAwayMessage(const std::string& msg) {
    awayMessage = msg;
    if (!msg.empty()) {
        userModes['a'] = true;
    } else {
        userModes.erase('a');
    }
}

const std::string& Client::getAwayMessage() const {
    return awayMessage;
}

bool Client::isAway() const {
    return !awayMessage.empty();
}

void Client::addToHistory(const std::string& message) {
    messageHistory.push_back(message);
    // Keep only last MAX_HISTORY messages
    while (messageHistory.size() > MAX_HISTORY) {
        messageHistory.pop_front();
    }
}

const std::deque<std::string>& Client::getHistory() const {
    return messageHistory;
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

// Rate limiting methods (Phase 3)
bool Client::checkRateLimit(int maxMessages, int timeWindowSeconds) {
    // Operators are exempt from rate limiting
    if (isOperator) {
        return true;
    }

    time_t now = time(nullptr);
    time_t cutoff = now - timeWindowSeconds;

    // Remove old timestamps outside the time window
    while (!messageTimes.empty() && messageTimes.front() < cutoff) {
        messageTimes.pop_front();
    }

    // Check if under limit
    return messageTimes.size() < static_cast<size_t>(maxMessages);
}

void Client::recordMessage() {
    time_t now = time(nullptr);
    messageTimes.push_back(now);
    totalMessageCount++;

    // Keep only last 100 timestamps to prevent memory issues
    while (messageTimes.size() > 100) {
        messageTimes.pop_front();
    }
}

size_t Client::getTotalMessageCount() const {
    return totalMessageCount;
}

time_t Client::getConnectionTime() const {
    return connectionTime;
}

size_t Client::getRecentMessageCount(int seconds) const {
    time_t now = time(nullptr);
    time_t cutoff = now - seconds;
    size_t count = 0;

    for (const auto& timestamp : messageTimes) {
        if (timestamp >= cutoff) {
            count++;
        }
    }

    return count;
}

#include "Channel.hpp"

Channel::Channel(const std::string& channelName)
    : name(channelName), userLimit(0) {
}

Channel::~Channel() {
}

const std::string& Channel::getName() const {
    return name;
}

const std::string& Channel::getTopic() const {
    return topic;
}

const std::set<int>& Channel::getMembers() const {
    return members;
}

size_t Channel::getMemberCount() const {
    return members.size();
}

bool Channel::hasMode(char mode) const {
    std::map<char, bool>::const_iterator it = modes.find(mode);
    if (it != modes.end()) {
        return it->second;
    }
    return false;
}

const std::string& Channel::getKey() const {
    return key;
}

size_t Channel::getUserLimit() const {
    return userLimit;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

void Channel::setMode(char mode, bool value) {
    modes[mode] = value;
}

void Channel::setKey(const std::string& newKey) {
    key = newKey;
}

void Channel::setUserLimit(size_t limit) {
    userLimit = limit;
}

void Channel::addMember(int clientFd) {
    members.insert(clientFd);
}

void Channel::removeMember(int clientFd) {
    members.erase(clientFd);
    operators.erase(clientFd);
}

bool Channel::hasMember(int clientFd) const {
    return members.find(clientFd) != members.end();
}

void Channel::addOperator(int clientFd) {
    if (hasMember(clientFd)) {
        operators.insert(clientFd);
    }
}

void Channel::removeOperator(int clientFd) {
    operators.erase(clientFd);
}

bool Channel::isOperator(int clientFd) const {
    return operators.find(clientFd) != operators.end();
}

void Channel::addInvite(int clientFd) {
    inviteList.insert(clientFd);
}

void Channel::removeInvite(int clientFd) {
    inviteList.erase(clientFd);
}

bool Channel::isInvited(int clientFd) const {
    return inviteList.find(clientFd) != inviteList.end();
}

bool Channel::isEmpty() const {
    return members.empty();
}

bool Channel::isFull() const {
    if (userLimit == 0) {
        return false;  // No limit
    }
    return members.size() >= userLimit;
}

#include "Channel.hpp"

Channel::Channel(const std::string& channelName) : name(channelName) {
}

Channel::~Channel() {
}

const std::string& Channel::getName() const { return name; }
const std::string& Channel::getTopic() const { return topic; }
const std::set<int>& Channel::getMembers() const { return members; }
size_t Channel::getMemberCount() const { return members.size(); }

void Channel::setTopic(const std::string& newTopic) { topic = newTopic; }

void Channel::addMember(int clientFd) { members.insert(clientFd); }

void Channel::removeMember(int clientFd) {
    members.erase(clientFd);
    operators.erase(clientFd);
}

bool Channel::hasMember(int clientFd) const {
    return members.find(clientFd) != members.end();
}

void Channel::addOperator(int clientFd) {
    if (hasMember(clientFd)) operators.insert(clientFd);
}

void Channel::removeOperator(int clientFd) { operators.erase(clientFd); }

bool Channel::isOperator(int clientFd) const {
    return operators.find(clientFd) != operators.end();
}

bool Channel::isEmpty() const { return members.empty(); }

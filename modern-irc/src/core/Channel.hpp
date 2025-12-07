#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

class Client;

class Channel {
private:
    std::string name;
    std::string topic;
    std::set<int> members;           // Client file descriptors
    std::set<int> operators;         // Channel operators
    std::set<int> inviteList;        // Invited users (for +i mode)
    std::map<char, bool> modes;      // Channel modes (+i, +t, etc.)
    std::string key;                 // Channel password (for +k mode)
    size_t userLimit;                // User limit (for +l mode)

public:
    Channel(const std::string& name);
    ~Channel();

    // Getters
    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::set<int>& getMembers() const;
    size_t getMemberCount() const;
    bool hasMode(char mode) const;
    const std::string& getKey() const;
    size_t getUserLimit() const;

    // Setters
    void setTopic(const std::string& topic);
    void setMode(char mode, bool value);
    void setKey(const std::string& key);
    void setUserLimit(size_t limit);

    // Member management
    void addMember(int clientFd);
    void removeMember(int clientFd);
    bool hasMember(int clientFd) const;

    // Operator management
    void addOperator(int clientFd);
    void removeOperator(int clientFd);
    bool isOperator(int clientFd) const;

    // Invite management
    void addInvite(int clientFd);
    void removeInvite(int clientFd);
    bool isInvited(int clientFd) const;

    // Checks
    bool isEmpty() const;
    bool isFull() const;
};

#endif

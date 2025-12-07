#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

class Channel {
private:
    std::string name;
    std::string topic;
    std::set<int> members;
    std::set<int> operators;

public:
    Channel(const std::string& name);
    ~Channel();

    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::set<int>& getMembers() const;
    size_t getMemberCount() const;

    void setTopic(const std::string& topic);

    void addMember(int clientFd);
    void removeMember(int clientFd);
    bool hasMember(int clientFd) const;

    void addOperator(int clientFd);
    void removeOperator(int clientFd);
    bool isOperator(int clientFd) const;

    bool isEmpty() const;
};

#endif

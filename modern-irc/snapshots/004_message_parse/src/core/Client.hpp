#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client {
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string hostname;
    std::set<std::string> channels;
    std::string recvBuffer;
    std::string sendBuffer;
    bool authenticated;
    bool registered;

public:
    Client(int fd);
    ~Client();

    // Getters
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    const std::set<std::string>& getChannels() const;
    bool isAuthenticated() const;
    bool isRegistered() const;

    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setHostname(const std::string& host);
    void setAuthenticated(bool auth);
    void setRegistered(bool reg);

    // Channel management
    void joinChannel(const std::string& channel);
    void leaveChannel(const std::string& channel);
    bool isInChannel(const std::string& channel) const;

    // Buffer management
    void appendToRecvBuffer(const std::string& data);
    std::string extractMessage();
    bool hasCompleteMessage() const;
    void appendToSendBuffer(const std::string& data);
    const std::string& getSendBuffer() const;
    void clearSendBuffer(size_t bytes);

    // Generate client prefix
    std::string getPrefix() const;
};

#endif

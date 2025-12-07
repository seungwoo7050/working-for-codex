#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>
#include <deque>
#include <map>
#include <ctime>

class Client {
private:
    int fd;
    std::string nickname;
    std::string username;
    std::string realname;
    std::string hostname;
    std::set<std::string> channels;  // Channels the client is in
    std::string recvBuffer;          // Buffer for incomplete messages
    std::string sendBuffer;          // Buffer for outgoing messages
    bool authenticated;              // Has provided correct password
    bool registered;                 // Has completed NICK + USER
    bool isOperator;                 // Server operator status
    std::map<char, bool> userModes;  // User modes (+i, +o, etc.)
    std::string awayMessage;         // Away message (for +a mode)
    std::deque<std::string> messageHistory;  // Recent private messages (max 50)
    static const size_t MAX_HISTORY = 50;

    // Rate limiting (Phase 3)
    std::deque<time_t> messageTimes;     // Timestamps of recent messages
    time_t connectionTime;               // When client connected
    size_t totalMessageCount;            // Total messages sent

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
    bool isOper() const;

    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setHostname(const std::string& host);
    void setAuthenticated(bool auth);
    void setRegistered(bool reg);
    void setOperator(bool oper);

    // User modes
    void setUserMode(char mode, bool value);
    bool hasUserMode(char mode) const;
    std::string getUserModes() const;
    void setAwayMessage(const std::string& msg);
    const std::string& getAwayMessage() const;
    bool isAway() const;

    // Message history
    void addToHistory(const std::string& message);
    const std::deque<std::string>& getHistory() const;

    // Rate limiting (Phase 3)
    bool checkRateLimit(int maxMessages, int timeWindowSeconds);
    void recordMessage();
    size_t getTotalMessageCount() const;
    time_t getConnectionTime() const;
    size_t getRecentMessageCount(int seconds) const;

    // Channel management
    void joinChannel(const std::string& channel);
    void leaveChannel(const std::string& channel);
    bool isInChannel(const std::string& channel) const;

    // Buffer management
    void appendToRecvBuffer(const std::string& data);
    std::string extractMessage();  // Extract one complete message from buffer
    bool hasCompleteMessage() const;
    void appendToSendBuffer(const std::string& data);
    const std::string& getSendBuffer() const;
    void clearSendBuffer(size_t bytes);

    // Generate client identifier for messages (nick!user@host)
    std::string getPrefix() const;
};

#endif

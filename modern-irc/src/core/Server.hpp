#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>
#include "../network/Socket.hpp"
#include "../network/Poller.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"

class Command;

// IRC numeric replies (RPL_*)
#define RPL_WELCOME 001
#define RPL_YOURHOST 002
#define RPL_CREATED 003
#define RPL_MYINFO 004
#define RPL_UMODEIS 221
#define RPL_AWAY 301
#define RPL_UNAWAY 305
#define RPL_NOWAWAY 306
#define RPL_WHOISUSER 311
#define RPL_WHOISSERVER 312
#define RPL_WHOISOPERATOR 313
#define RPL_WHOISIDLE 317
#define RPL_ENDOFWHOIS 318
#define RPL_WHOISCHANNELS 319
#define RPL_CHANNELMODEIS 324
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341
#define RPL_WHOREPLY 352
#define RPL_ENDOFWHO 315
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_LIST 322
#define RPL_LISTEND 323

// IRC error codes (ERR_*)
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHSERVER 402
#define ERR_NOSUCHCHANNEL 403
#define ERR_CANNOTSENDTOCHAN 404
#define ERR_TOOMANYCHANNELS 405
#define ERR_WASNOSUCHNICK 406
#define ERR_TOOMANYTARGETS 407
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NOMOTD 422
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_USERONCHANNEL 443
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_YOUREBANNEDCREEP 465
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_NOPRIVILEGES 481
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502

class Server {
private:
    std::string serverName;
    std::string password;
    std::string operPassword;  // Phase 3: Operator password
    int port;
    Socket serverSocket;
    Poller poller;
    std::map<int, std::unique_ptr<Client>> clients;  // fd -> Client
    std::map<std::string, std::unique_ptr<Channel>> channels;  // channel name -> Channel
    std::map<std::string, std::unique_ptr<Command>> commands;  // command name -> Command handler
    bool running;

    // Phase 3: Server statistics
    time_t startTime;
    size_t totalConnections;
    size_t totalMessages;
    size_t peakClientCount;
    size_t totalChannelsCreated;

    // Helper methods
    void acceptNewClient();
    void handleClientData(int clientFd);
    void handleClientDisconnect(int clientFd);
    void processMessage(Client& client, const Message& message);
    void registerCommands();

public:
    Server(int port, const std::string& password);
    ~Server();

    // Main loop
    bool initialize();
    void run();
    void stop();

    // Client management
    Client* getClient(int fd);
    Client* getClientByNick(const std::string& nickname);
    void removeClient(int fd);
    void sendToClient(Client& client, const std::string& message);
    void sendToClient(int fd, const std::string& message);

    // Channel management
    Channel* getChannel(const std::string& name);
    Channel* createChannel(const std::string& name);
    void removeChannel(const std::string& name);
    std::vector<std::string> getChannelList() const;
    void loadChannelTopics();
    void saveChannelTopic(const std::string& channelName, const std::string& topic);

    // Message broadcasting
    void broadcastToChannel(const std::string& channelName, const std::string& message, int excludeFd = -1);

    // Helpers
    std::string getServerName() const;
    std::string getPassword() const;
    std::string getOperPassword() const;
    void setOperPassword(const std::string& pass);
    bool isNicknameInUse(const std::string& nickname) const;
    std::string formatReply(int code, const Client& client, const std::string& message) const;
    std::string formatReply(int code, const Client& client, const std::string& param, const std::string& message) const;

    // Phase 3: Statistics
    void incrementMessageCount();
    void updatePeakClientCount();
    time_t getStartTime() const;
    size_t getTotalConnections() const;
    size_t getTotalMessages() const;
    size_t getPeakClientCount() const;
    size_t getTotalChannelsCreated() const;
    size_t getCurrentClientCount() const;
    size_t getCurrentChannelCount() const;
    std::map<int, std::unique_ptr<Client>>& getClients();

    // Phase 3: Config reload
    bool reloadConfiguration(const std::string& configPath);
};

#endif

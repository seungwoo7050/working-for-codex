#include "ServerStatsCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Message.hpp"
#include <sstream>
#include <ctime>

// Numeric reply codes for stats
#define RPL_STATSUPTIME 242
#define RPL_ENDOFSTATS 219
#define RPL_INFO 371
#define RPL_ENDOFINFO 374
#define RPL_VERSION 351
#define RPL_TIME 391
#define RPL_LUSERCLIENT 251
#define RPL_LUSEROP 252
#define RPL_LUSERUNKNOWN 253
#define RPL_LUSERCHANNELS 254
#define RPL_LUSERME 255

// STATS command - Server statistics
void StatsCommand::execute(Server& server, Client& client, const Message& msg) {
    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    char query = 'u';  // Default to uptime
    if (msg.getParamCount() >= 1 && !msg.getParam(0).empty()) {
        query = msg.getParam(0)[0];
    }

    std::ostringstream oss;

    switch (query) {
        case 'u': {  // Uptime
            time_t now = time(nullptr);
            time_t uptime = now - server.getStartTime();
            int days = uptime / 86400;
            int hours = (uptime % 86400) / 3600;
            int minutes = (uptime % 3600) / 60;
            int seconds = uptime % 60;

            oss << ":" << server.getServerName() << " " << RPL_STATSUPTIME << " " << client.getNickname()
                << " :Server Up " << days << " days " << hours << ":" << minutes << ":" << seconds << "\r\n";
            server.sendToClient(client, oss.str());
            break;
        }

        case 'm': {  // Message statistics
            oss << ":" << server.getServerName() << " 212 " << client.getNickname()
                << " :Total messages: " << server.getTotalMessages() << "\r\n";
            server.sendToClient(client, oss.str());
            break;
        }

        case 'c': {  // Connection statistics
            oss << ":" << server.getServerName() << " 213 " << client.getNickname()
                << " :Current clients: " << server.getCurrentClientCount() << "\r\n";
            server.sendToClient(client, oss.str());

            oss.str("");
            oss << ":" << server.getServerName() << " 213 " << client.getNickname()
                << " :Total connections: " << server.getTotalConnections() << "\r\n";
            server.sendToClient(client, oss.str());

            oss.str("");
            oss << ":" << server.getServerName() << " 213 " << client.getNickname()
                << " :Peak clients: " << server.getPeakClientCount() << "\r\n";
            server.sendToClient(client, oss.str());
            break;
        }

        case 'l': {  // Client list (operators only)
            if (!client.isOper()) {
                server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Permission Denied"));
                return;
            }

            auto& clients = server.getClients();
            for (const auto& [fd, clientPtr] : clients) {
                time_t connTime = time(nullptr) - clientPtr->getConnectionTime();

                oss.str("");
                oss << ":" << server.getServerName() << " 211 " << client.getNickname()
                    << " " << clientPtr->getNickname() << " " << clientPtr->getHostname()
                    << " " << connTime << " " << clientPtr->getTotalMessageCount() << "\r\n";
                server.sendToClient(client, oss.str());
            }
            break;
        }
    }

    // Send end of stats
    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_ENDOFSTATS << " " << client.getNickname()
        << " " << query << " :End of STATS report\r\n";
    server.sendToClient(client, oss.str());
}

// INFO command - Server information
void InfoCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    std::ostringstream oss;

    // Send server info
    oss << ":" << server.getServerName() << " " << RPL_INFO << " " << client.getNickname()
        << " :modern-irc - Phase 3 Implementation\r\n";
    server.sendToClient(client, oss.str());

    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_INFO << " " << client.getNickname()
        << " :Based on RFC 1459, 2810-2813\r\n";
    server.sendToClient(client, oss.str());

    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_INFO << " " << client.getNickname()
        << " :Supports: Channels, Modes, Operator Commands, Rate Limiting\r\n";
    server.sendToClient(client, oss.str());

    // Runtime statistics
    time_t uptime = time(nullptr) - server.getStartTime();
    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_INFO << " " << client.getNickname()
        << " :Server uptime: " << (uptime / 86400) << " days, " << ((uptime % 86400) / 3600) << " hours\r\n";
    server.sendToClient(client, oss.str());

    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_INFO << " " << client.getNickname()
        << " :Current users: " << server.getCurrentClientCount()
        << ", Peak: " << server.getPeakClientCount() << "\r\n";
    server.sendToClient(client, oss.str());

    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_INFO << " " << client.getNickname()
        << " :Channels: " << server.getCurrentChannelCount()
        << ", Total created: " << server.getTotalChannelsCreated() << "\r\n";
    server.sendToClient(client, oss.str());

    // End of info
    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_ENDOFINFO << " " << client.getNickname()
        << " :End of INFO\r\n";
    server.sendToClient(client, oss.str());
}

// VERSION command
void VersionCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    std::ostringstream oss;
    oss << ":" << server.getServerName() << " " << RPL_VERSION << " " << client.getNickname()
        << " 3.0.0 " << server.getServerName() << " :Phase 3 - Full modern-irc\r\n";
    server.sendToClient(client, oss.str());
}

// TIME command
void TimeCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%A %B %d %Y -- %H:%M:%S %z", timeinfo);

    std::ostringstream oss;
    oss << ":" << server.getServerName() << " " << RPL_TIME << " " << client.getNickname()
        << " " << server.getServerName() << " :" << buffer << "\r\n";
    server.sendToClient(client, oss.str());
}

// LUSERS command - User statistics
void LusersCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    size_t totalUsers = server.getCurrentClientCount();
    size_t operCount = 0;

    // Count operators
    auto& clients = server.getClients();
    for (const auto& [fd, clientPtr] : clients) {
        if (clientPtr->isOper()) {
            operCount++;
        }
    }

    std::ostringstream oss;

    // RPL_LUSERCLIENT
    oss << ":" << server.getServerName() << " " << RPL_LUSERCLIENT << " " << client.getNickname()
        << " :There are " << totalUsers << " users and 0 services on 1 servers\r\n";
    server.sendToClient(client, oss.str());

    // RPL_LUSEROP
    if (operCount > 0) {
        oss.str("");
        oss << ":" << server.getServerName() << " " << RPL_LUSEROP << " " << client.getNickname()
            << " " << operCount << " :operator(s) online\r\n";
        server.sendToClient(client, oss.str());
    }

    // RPL_LUSERUNKNOWN
    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_LUSERUNKNOWN << " " << client.getNickname()
        << " 0 :unknown connection(s)\r\n";
    server.sendToClient(client, oss.str());

    // RPL_LUSERCHANNELS
    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_LUSERCHANNELS << " " << client.getNickname()
        << " " << server.getCurrentChannelCount() << " :channels formed\r\n";
    server.sendToClient(client, oss.str());

    // RPL_LUSERME
    oss.str("");
    oss << ":" << server.getServerName() << " " << RPL_LUSERME << " " << client.getNickname()
        << " :I have " << totalUsers << " clients and 0 servers\r\n";
    server.sendToClient(client, oss.str());
}

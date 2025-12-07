#include "AdminCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Message.hpp"
#include "../utils/Logger.hpp"
#include <sstream>

// OPER command - Gain operator privileges
void OperCommand::execute(Server& server, Client& client, const Message& msg) {
    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    if (msg.getParamCount() < 2) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "OPER", "Not enough parameters"));
        return;
    }

    std::string username = msg.getParam(0);
    std::string password = msg.getParam(1);

    // Check if password matches
    if (password != server.getOperPassword()) {
        server.sendToClient(client, server.formatReply(ERR_PASSWDMISMATCH, client, "Password incorrect"));
        Logger::getInstance().warning("Failed OPER attempt from " + client.getNickname());
        return;
    }

    // Grant operator status
    client.setOperator(true);
    server.sendToClient(client, server.formatReply(381, client, "You are now an IRC operator"));

    std::ostringstream oss;
    oss << ":" << client.getPrefix() << " MODE " << client.getNickname() << " :+o\r\n";
    server.sendToClient(client, oss.str());

    Logger::getInstance().info("User " + client.getNickname() + " is now an operator");
}

// KILL command - Forcibly disconnect a client
void KillCommand::execute(Server& server, Client& client, const Message& msg) {
    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    if (!client.isOper()) {
        server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Permission Denied- You're not an IRC operator"));
        return;
    }

    if (msg.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "KILL", "Not enough parameters"));
        return;
    }

    std::string targetNick = msg.getParam(0);
    std::string reason = msg.getParamCount() >= 2 ? msg.getParam(1) : "Killed by operator";

    auto* target = server.getClientByNick(targetNick);
    if (!target) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHNICK, client, targetNick, "No such nick/channel"));
        return;
    }

    // Cannot kill other operators
    if (target->isOper()) {
        server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Cannot kill other operators"));
        return;
    }

    // Send KILL message to target
    std::ostringstream oss;
    oss << ":" << client.getPrefix() << " KILL " << targetNick << " :" << reason << "\r\n";
    server.sendToClient(*target, oss.str());

    // Send ERROR to target
    oss.str("");
    oss << "ERROR :Closing Link: " << target->getHostname() << " (Killed (" << client.getNickname() << " (" << reason << ")))\r\n";
    server.sendToClient(*target, oss.str());

    Logger::getInstance().info("Operator " + client.getNickname() + " killed " + targetNick + " (" + reason + ")");

    // Disconnect the target
    int targetFd = target->getFd();
    server.removeClient(targetFd);
}

// REHASH command - Reload configuration
void RehashCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    if (!client.isOper()) {
        server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Permission Denied- You're not an IRC operator"));
        return;
    }

    // Reload configuration
    Logger::getInstance().info("Operator " + client.getNickname() + " requested configuration reload");

    // In a real implementation, this would reload the config file
    server.sendToClient(client, server.formatReply(382, client, "config/modern-irc.conf", "Rehashing"));

    Logger::getInstance().info("Configuration reloaded");
}

// RESTART command - Restart the server
void RestartCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    if (!client.isOper()) {
        server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Permission Denied- You're not an IRC operator"));
        return;
    }

    Logger::getInstance().critical("Operator " + client.getNickname() + " requested server restart");

    // Broadcast to all clients
    auto& clients = server.getClients();
    for (auto& [fd, clientPtr] : clients) {
        server.sendToClient(*clientPtr, "ERROR :Server restarting by operator request\r\n");
    }

    // In a real implementation, this would gracefully restart the server
    // For now, we just log it
    server.sendToClient(client, "NOTICE " + client.getNickname() + " :Server restart initiated (simulation mode)\r\n");
}

// DIE command - Shutdown the server
void DieCommand::execute(Server& server, Client& client, const Message& msg) {
    (void)msg;  // Unused parameter

    if (!client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_NOTREGISTERED, client, "You have not registered"));
        return;
    }

    if (!client.isOper()) {
        server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Permission Denied- You're not an IRC operator"));
        return;
    }

    Logger::getInstance().critical("Operator " + client.getNickname() + " requested server shutdown");

    // Broadcast to all clients
    auto& clients = server.getClients();
    for (auto& [fd, clientPtr] : clients) {
        server.sendToClient(*clientPtr, "ERROR :Server shutting down by operator request\r\n");
    }

    // Stop the server
    server.stop();
}

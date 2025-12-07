#include "AuthCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Message.hpp"
#include "../utils/StringUtils.hpp"
#include <sstream>

// PASS command
void PassCommand::execute(Server& server, Client& client, const Message& message) {
    if (client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_ALREADYREGISTRED, client, "You may not reregister"));
        return;
    }

    if (message.getParamCount() < 1 && message.getTrailing().empty()) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "PASS", "Not enough parameters"));
        return;
    }

    std::string password = message.getParamCount() > 0 ? message.getParam(0) : message.getTrailing();

    if (password == server.getPassword()) {
        client.setAuthenticated(true);
    } else {
        server.sendToClient(client, server.formatReply(ERR_PASSWDMISMATCH, client, "Password incorrect"));
    }
}

// NICK command
void NickCommand::execute(Server& server, Client& client, const Message& message) {
    if (message.getParamCount() < 1 && message.getTrailing().empty()) {
        server.sendToClient(client, server.formatReply(ERR_NONICKNAMEGIVEN, client, "No nickname given"));
        return;
    }

    std::string newNick = message.getParamCount() > 0 ? message.getParam(0) : message.getTrailing();

    // Validate nickname
    if (newNick.empty() || newNick.length() > 9) {
        server.sendToClient(client, server.formatReply(ERR_ERRONEUSNICKNAME, client, newNick, "Erroneous nickname"));
        return;
    }

    // Check first character
    if (!StringUtils::isValidNickChar(newNick[0], true)) {
        server.sendToClient(client, server.formatReply(ERR_ERRONEUSNICKNAME, client, newNick, "Erroneous nickname"));
        return;
    }

    // Check remaining characters
    for (size_t i = 1; i < newNick.length(); ++i) {
        if (!StringUtils::isValidNickChar(newNick[i], false)) {
            server.sendToClient(client, server.formatReply(ERR_ERRONEUSNICKNAME, client, newNick, "Erroneous nickname"));
            return;
        }
    }

    // Check if nickname is already in use
    if (server.isNicknameInUse(newNick)) {
        server.sendToClient(client, server.formatReply(ERR_NICKNAMEINUSE, client, newNick, "Nickname is already in use"));
        return;
    }

    // Set nickname
    std::string oldNick = client.getNickname();
    client.setNickname(newNick);

    // If already registered, notify about nick change
    if (client.isRegistered()) {
        Message reply;
        reply.setPrefix(oldNick.empty() ? "*" : oldNick);
        reply.setCommand("NICK");
        reply.addParam(newNick);
        server.sendToClient(client, reply.toString());
    }

    // Check if registration is complete
    if (!client.isRegistered() && client.isAuthenticated() &&
        !client.getNickname().empty() && !client.getUsername().empty()) {
        client.setRegistered(true);
        std::string welcomeMsg = "Welcome to the IRC Network " + client.getPrefix();
        server.sendToClient(client, server.formatReply(RPL_WELCOME, client, welcomeMsg));
    }
}

// USER command
void UserCommand::execute(Server& server, Client& client, const Message& message) {
    if (client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_ALREADYREGISTRED, client, "You may not reregister"));
        return;
    }

    if (message.getParamCount() < 3 || message.getTrailing().empty()) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "USER", "Not enough parameters"));
        return;
    }

    std::string username = message.getParam(0);
    std::string realname = message.getTrailing();

    client.setUsername(username);
    client.setRealname(realname);

    // Check if registration is complete
    if (!client.isRegistered() && client.isAuthenticated() &&
        !client.getNickname().empty() && !client.getUsername().empty()) {
        client.setRegistered(true);
        std::string welcomeMsg = "Welcome to the IRC Network " + client.getPrefix();
        server.sendToClient(client, server.formatReply(RPL_WELCOME, client, welcomeMsg));
    }
}

// QUIT command
void QuitCommand::execute(Server& server, Client& client, const Message& message) {
    std::string quitMsg = message.getTrailing().empty() ? "Client quit" : message.getTrailing();

    // Notify all channels the client is in
    const std::set<std::string>& channels = client.getChannels();
    for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it) {
        Message reply;
        reply.setPrefix(client.getPrefix());
        reply.setCommand("QUIT");
        reply.setTrailing(quitMsg);
        server.broadcastToChannel(*it, reply.toString(), client.getFd());
    }

    // Remove client
    server.removeClient(client.getFd());
}

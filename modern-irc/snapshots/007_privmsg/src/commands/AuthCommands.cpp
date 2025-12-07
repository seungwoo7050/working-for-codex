#include "AuthCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Message.hpp"
#include "../utils/StringUtils.hpp"

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

void NickCommand::execute(Server& server, Client& client, const Message& message) {
    if (message.getParamCount() < 1 && message.getTrailing().empty()) {
        server.sendToClient(client, server.formatReply(ERR_NONICKNAMEGIVEN, client, "No nickname given"));
        return;
    }

    std::string newNick = message.getParamCount() > 0 ? message.getParam(0) : message.getTrailing();

    if (newNick.empty() || newNick.length() > 9) {
        server.sendToClient(client, server.formatReply(ERR_ERRONEUSNICKNAME, client, newNick, "Erroneous nickname"));
        return;
    }

    if (!StringUtils::isValidNickChar(newNick[0], true)) {
        server.sendToClient(client, server.formatReply(ERR_ERRONEUSNICKNAME, client, newNick, "Erroneous nickname"));
        return;
    }

    for (size_t i = 1; i < newNick.length(); ++i) {
        if (!StringUtils::isValidNickChar(newNick[i], false)) {
            server.sendToClient(client, server.formatReply(ERR_ERRONEUSNICKNAME, client, newNick, "Erroneous nickname"));
            return;
        }
    }

    if (server.isNicknameInUse(newNick)) {
        server.sendToClient(client, server.formatReply(ERR_NICKNAMEINUSE, client, newNick, "Nickname is already in use"));
        return;
    }

    std::string oldNick = client.getNickname();
    client.setNickname(newNick);

    if (client.isRegistered()) {
        Message reply;
        reply.setPrefix(oldNick.empty() ? "*" : oldNick);
        reply.setCommand("NICK");
        reply.addParam(newNick);
        server.sendToClient(client, reply.toString());
    }

    if (!client.isRegistered() && client.isAuthenticated() &&
        !client.getNickname().empty() && !client.getUsername().empty()) {
        client.setRegistered(true);
        server.sendToClient(client, server.formatReply(RPL_WELCOME, client,
            "Welcome to the IRC Network " + client.getPrefix()));
    }
}

void UserCommand::execute(Server& server, Client& client, const Message& message) {
    if (client.isRegistered()) {
        server.sendToClient(client, server.formatReply(ERR_ALREADYREGISTRED, client, "You may not reregister"));
        return;
    }

    if (message.getParamCount() < 3 || message.getTrailing().empty()) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "USER", "Not enough parameters"));
        return;
    }

    client.setUsername(message.getParam(0));
    client.setRealname(message.getTrailing());

    if (!client.isRegistered() && client.isAuthenticated() &&
        !client.getNickname().empty() && !client.getUsername().empty()) {
        client.setRegistered(true);
        server.sendToClient(client, server.formatReply(RPL_WELCOME, client,
            "Welcome to the IRC Network " + client.getPrefix()));
    }
}

void QuitCommand::execute(Server& server, Client& client, const Message& message) {
    std::string quitMsg = message.getTrailing().empty() ? "Client quit" : message.getTrailing();

    const std::set<std::string>& channels = client.getChannels();
    for (const auto& channelName : channels) {
        Message reply;
        reply.setPrefix(client.getPrefix());
        reply.setCommand("QUIT");
        reply.setTrailing(quitMsg);
        server.broadcastToChannel(channelName, reply.toString(), client.getFd());
    }

    server.removeClient(client.getFd());
}

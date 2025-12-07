#include "MessageCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Channel.hpp"
#include "../core/Message.hpp"
#include "../utils/StringUtils.hpp"

void PrivmsgCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) return;

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NORECIPIENT, client, "No recipient given (PRIVMSG)"));
        return;
    }

    if (message.getTrailing().empty() && message.getParamCount() < 2) {
        server.sendToClient(client, server.formatReply(ERR_NOTEXTTOSEND, client, "No text to send"));
        return;
    }

    std::string target = message.getParam(0);
    std::string text = message.getTrailing().empty() ? message.getParam(1) : message.getTrailing();

    if (StringUtils::isValidChannelChar(target[0], true)) {
        Channel* channel = server.getChannel(target);
        if (!channel) {
            server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, target, "No such channel"));
            return;
        }

        if (!channel->hasMember(client.getFd())) {
            server.sendToClient(client, server.formatReply(ERR_CANNOTSENDTOCHAN, client, target, "Cannot send to channel"));
            return;
        }

        Message privmsg;
        privmsg.setPrefix(client.getPrefix());
        privmsg.setCommand("PRIVMSG");
        privmsg.addParam(target);
        privmsg.setTrailing(text);
        server.broadcastToChannel(target, privmsg.toString(), client.getFd());
    } else {
        Client* targetClient = server.getClientByNick(target);
        if (!targetClient) {
            server.sendToClient(client, server.formatReply(ERR_NOSUCHNICK, client, target, "No such nick/channel"));
            return;
        }

        Message privmsg;
        privmsg.setPrefix(client.getPrefix());
        privmsg.setCommand("PRIVMSG");
        privmsg.addParam(target);
        privmsg.setTrailing(text);
        server.sendToClient(*targetClient, privmsg.toString());

        client.addToHistory("[to " + target + "] " + text);
        targetClient->addToHistory("[from " + client.getNickname() + "] " + text);
    }
}

void NoticeCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) return;

    if (message.getParamCount() < 1 || (message.getTrailing().empty() && message.getParamCount() < 2)) {
        return;  // NOTICE doesn't send error replies
    }

    std::string target = message.getParam(0);
    std::string text = message.getTrailing().empty() ? message.getParam(1) : message.getTrailing();

    if (StringUtils::isValidChannelChar(target[0], true)) {
        Channel* channel = server.getChannel(target);
        if (channel && channel->hasMember(client.getFd())) {
            Message notice;
            notice.setPrefix(client.getPrefix());
            notice.setCommand("NOTICE");
            notice.addParam(target);
            notice.setTrailing(text);
            server.broadcastToChannel(target, notice.toString(), client.getFd());
        }
    } else {
        Client* targetClient = server.getClientByNick(target);
        if (targetClient) {
            Message notice;
            notice.setPrefix(client.getPrefix());
            notice.setCommand("NOTICE");
            notice.addParam(target);
            notice.setTrailing(text);
            server.sendToClient(*targetClient, notice.toString());
        }
    }
}

void PingCommand::execute(Server& server, Client& client, const Message& message) {
    std::string token = message.getParamCount() > 0 ? message.getParam(0) : message.getTrailing();

    Message pong;
    pong.setPrefix(server.getServerName());
    pong.setCommand("PONG");
    pong.setTrailing(token);
    server.sendToClient(client, pong.toString());
}

void PongCommand::execute(Server& server, Client& client, const Message& message) {
    (void)server;
    (void)client;
    (void)message;
}

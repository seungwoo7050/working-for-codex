#include "ChannelCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Channel.hpp"
#include "../core/Message.hpp"
#include "../utils/StringUtils.hpp"
#include <sstream>

void JoinCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) return;

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "JOIN", "Not enough parameters"));
        return;
    }

    std::string channelName = message.getParam(0);

    if (channelName.empty() || !StringUtils::isValidChannelChar(channelName[0], true)) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, channelName, "No such channel"));
        return;
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        channel = server.createChannel(channelName);
        channel->addMember(client.getFd());
        channel->addOperator(client.getFd());
    } else {
        channel->addMember(client.getFd());
    }

    client.joinChannel(channelName);

    Message joinMsg;
    joinMsg.setPrefix(client.getPrefix());
    joinMsg.setCommand("JOIN");
    joinMsg.addParam(channelName);
    server.broadcastToChannel(channelName, joinMsg.toString());

    if (!channel->getTopic().empty()) {
        server.sendToClient(client, server.formatReply(RPL_TOPIC, client, channelName, channel->getTopic()));
    } else {
        server.sendToClient(client, server.formatReply(RPL_NOTOPIC, client, channelName, "No topic is set"));
    }

    std::ostringstream names;
    const auto& members = channel->getMembers();
    for (int memberFd : members) {
        auto* member = server.getClient(memberFd);
        if (member) {
            if (channel->isOperator(memberFd)) names << "@";
            names << member->getNickname() << " ";
        }
    }

    server.sendToClient(client, server.formatReply(RPL_NAMREPLY, client, "= " + channelName, names.str()));
    server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, channelName, "End of /NAMES list"));
}

void PartCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) return;

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "PART", "Not enough parameters"));
        return;
    }

    std::string channelName = message.getParam(0);
    std::string partMsg = message.getTrailing().empty() ? "Leaving" : message.getTrailing();

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, channelName, "No such channel"));
        return;
    }

    if (!channel->hasMember(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_NOTONCHANNEL, client, channelName, "You're not on that channel"));
        return;
    }

    Message partMsgObj;
    partMsgObj.setPrefix(client.getPrefix());
    partMsgObj.setCommand("PART");
    partMsgObj.addParam(channelName);
    partMsgObj.setTrailing(partMsg);
    server.broadcastToChannel(channelName, partMsgObj.toString());

    channel->removeMember(client.getFd());
    client.leaveChannel(channelName);

    if (channel->isEmpty()) {
        server.removeChannel(channelName);
    }
}

void NamesCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) return;

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, "*", "End of /NAMES list"));
        return;
    }

    std::string channelName = message.getParam(0);
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, channelName, "End of /NAMES list"));
        return;
    }

    std::ostringstream names;
    const auto& members = channel->getMembers();
    for (int memberFd : members) {
        auto* member = server.getClient(memberFd);
        if (member) {
            if (channel->isOperator(memberFd)) names << "@";
            names << member->getNickname() << " ";
        }
    }

    server.sendToClient(client, server.formatReply(RPL_NAMREPLY, client, "= " + channelName, names.str()));
    server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, channelName, "End of /NAMES list"));
}

void ListCommand::execute(Server& server, Client& client, const Message& message) {
    (void)message;
    if (!client.isRegistered()) return;

    std::vector<std::string> channels = server.getChannelList();

    for (size_t i = 0; i < channels.size(); ++i) {
        Channel* channel = server.getChannel(channels[i]);
        if (channel) {
            std::ostringstream info;
            info << channel->getMemberCount();
            std::string topic = channel->getTopic().empty() ? "No topic" : channel->getTopic();
            server.sendToClient(client, server.formatReply(RPL_LIST, client, channels[i] + " " + info.str(), topic));
        }
    }
    server.sendToClient(client, server.formatReply(RPL_LISTEND, client, "End of /LIST"));
}

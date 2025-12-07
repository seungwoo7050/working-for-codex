#include "ChannelCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Channel.hpp"
#include "../core/Message.hpp"
#include "../utils/StringUtils.hpp"
#include "../utils/TopicPersistence.hpp"
#include <sstream>

// JOIN command
void JoinCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;  // Silently ignore if not registered
    }

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "JOIN", "Not enough parameters"));
        return;
    }

    std::string channelName = message.getParam(0);

    // Validate channel name
    if (channelName.empty() || !StringUtils::isValidChannelChar(channelName[0], true)) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, channelName, "No such channel"));
        return;
    }

    // Get or create channel
    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        channel = server.createChannel(channelName);
        // First user becomes operator
        channel->addMember(client.getFd());
        channel->addOperator(client.getFd());

        // Load persisted topic (Phase 2 feature)
        std::string savedTopic = TopicPersistence::loadTopic(channelName);
        if (!savedTopic.empty()) {
            channel->setTopic(savedTopic);
        }
    } else {
        // Check if channel is full
        if (channel->isFull()) {
            server.sendToClient(client, server.formatReply(ERR_CHANNELISFULL, client, channelName, "Cannot join channel (+l)"));
            return;
        }

        // Check if invite-only
        if (channel->hasMode('i') && !channel->isInvited(client.getFd())) {
            server.sendToClient(client, server.formatReply(ERR_INVITEONLYCHAN, client, channelName, "Cannot join channel (+i)"));
            return;
        }

        // Check key if set
        if (channel->hasMode('k')) {
            std::string key = message.getParamCount() > 1 ? message.getParam(1) : "";
            if (key != channel->getKey()) {
                server.sendToClient(client, server.formatReply(ERR_BADCHANNELKEY, client, channelName, "Cannot join channel (+k)"));
                return;
            }
        }

        channel->addMember(client.getFd());
        // Remove from invite list after joining
        channel->removeInvite(client.getFd());
    }

    client.joinChannel(channelName);

    // Notify all members including the joining user
    Message joinMsg;
    joinMsg.setPrefix(client.getPrefix());
    joinMsg.setCommand("JOIN");
    joinMsg.addParam(channelName);
    server.broadcastToChannel(channelName, joinMsg.toString());

    // Send topic if set
    if (!channel->getTopic().empty()) {
        server.sendToClient(client, server.formatReply(RPL_TOPIC, client, channelName, channel->getTopic()));
    } else {
        server.sendToClient(client, server.formatReply(RPL_NOTOPIC, client, channelName, "No topic is set"));
    }

    // Send NAMES list
    std::ostringstream names;
    const auto& members = channel->getMembers();
    for (int memberFd : members) {
        auto* member = server.getClient(memberFd);
        if (member) {
            if (channel->isOperator(memberFd)) {
                names << "@";
            }
            names << member->getNickname() << " ";
        }
    }

    server.sendToClient(client, server.formatReply(RPL_NAMREPLY, client, "= " + channelName, names.str()));
    server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, channelName, "End of /NAMES list"));
}

// PART command
void PartCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

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

    // Notify all members including the parting user
    Message partMsgObj;
    partMsgObj.setPrefix(client.getPrefix());
    partMsgObj.setCommand("PART");
    partMsgObj.addParam(channelName);
    partMsgObj.setTrailing(partMsg);
    server.broadcastToChannel(channelName, partMsgObj.toString());

    // Remove from channel
    channel->removeMember(client.getFd());
    client.leaveChannel(channelName);

    // Remove channel if empty
    if (channel->isEmpty()) {
        server.removeChannel(channelName);
    }
}

// NAMES command
void NamesCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    if (message.getParamCount() < 1) {
        // List all channels (simplified - just end)
        server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, "*", "End of /NAMES list"));
        return;
    }

    std::string channelName = message.getParam(0);
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, channelName, "End of /NAMES list"));
        return;
    }

    // Build names list
    std::ostringstream names;
    const auto& members = channel->getMembers();
    for (int memberFd : members) {
        auto* member = server.getClient(memberFd);
        if (member) {
            if (channel->isOperator(memberFd)) {
                names << "@";
            }
            names << member->getNickname() << " ";
        }
    }

    server.sendToClient(client, server.formatReply(RPL_NAMREPLY, client, "= " + channelName, names.str()));
    server.sendToClient(client, server.formatReply(RPL_ENDOFNAMES, client, channelName, "End of /NAMES list"));
}

// LIST command
void ListCommand::execute(Server& server, Client& client, const Message& message) {
    (void)message;  // Unused parameter

    if (!client.isRegistered()) {
        return;
    }

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

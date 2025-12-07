#include "OperatorCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Channel.hpp"
#include "../core/Message.hpp"
#include "../utils/StringUtils.hpp"
#include <sstream>

// KICK command - remove user from channel (operators only)
void KickCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    if (message.getParamCount() < 2) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "KICK", "Not enough parameters"));
        return;
    }

    std::string channelName = message.getParam(0);
    std::string targetNick = message.getParam(1);
    std::string reason = message.getTrailing().empty() ? client.getNickname() : message.getTrailing();

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, channelName, "No such channel"));
        return;
    }

    if (!channel->hasMember(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_NOTONCHANNEL, client, channelName, "You're not on that channel"));
        return;
    }

    if (!channel->isOperator(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_CHANOPRIVSNEEDED, client, channelName, "You're not channel operator"));
        return;
    }

    Client* target = server.getClientByNick(targetNick);
    if (!target) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHNICK, client, targetNick, "No such nick/channel"));
        return;
    }

    if (!channel->hasMember(target->getFd())) {
        server.sendToClient(client, server.formatReply(ERR_USERNOTINCHANNEL, client, targetNick + " " + channelName, "They aren't on that channel"));
        return;
    }

    // Broadcast KICK message to all channel members
    Message kickMsg;
    kickMsg.setPrefix(client.getPrefix());
    kickMsg.setCommand("KICK");
    kickMsg.addParam(channelName);
    kickMsg.addParam(targetNick);
    kickMsg.setTrailing(reason);
    server.broadcastToChannel(channelName, kickMsg.toString());

    // Remove the user from the channel
    channel->removeMember(target->getFd());
    target->leaveChannel(channelName);

    // Remove channel if empty
    if (channel->isEmpty()) {
        server.removeChannel(channelName);
    }
}

// INVITE command - invite user to channel
void InviteCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    if (message.getParamCount() < 2) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "INVITE", "Not enough parameters"));
        return;
    }

    std::string targetNick = message.getParam(0);
    std::string channelName = message.getParam(1);

    Client* target = server.getClientByNick(targetNick);
    if (!target) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHNICK, client, targetNick, "No such nick/channel"));
        return;
    }

    Channel* channel = server.getChannel(channelName);
    if (!channel) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, channelName, "No such channel"));
        return;
    }

    if (!channel->hasMember(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_NOTONCHANNEL, client, channelName, "You're not on that channel"));
        return;
    }

    if (channel->hasMember(target->getFd())) {
        server.sendToClient(client, server.formatReply(ERR_USERONCHANNEL, client, targetNick + " " + channelName, "is already on channel"));
        return;
    }

    // Check if channel is invite-only and user is not operator
    if (channel->hasMode('i') && !channel->isOperator(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_CHANOPRIVSNEEDED, client, channelName, "You're not channel operator"));
        return;
    }

    // Add to invite list
    channel->addInvite(target->getFd());

    // Notify inviter
    server.sendToClient(client, server.formatReply(RPL_INVITING, client, targetNick + " " + channelName, ""));

    // Send INVITE to target
    Message inviteMsg;
    inviteMsg.setPrefix(client.getPrefix());
    inviteMsg.setCommand("INVITE");
    inviteMsg.addParam(targetNick);
    inviteMsg.setTrailing(channelName);
    server.sendToClient(*target, inviteMsg.toString());
}

// TOPIC command - view or set channel topic
void TopicCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "TOPIC", "Not enough parameters"));
        return;
    }

    std::string channelName = message.getParam(0);
    Channel* channel = server.getChannel(channelName);

    if (!channel) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, channelName, "No such channel"));
        return;
    }

    if (!channel->hasMember(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_NOTONCHANNEL, client, channelName, "You're not on that channel"));
        return;
    }

    // If no topic parameter, just display current topic
    if (message.getTrailing().empty() && message.getParamCount() == 1) {
        if (channel->getTopic().empty()) {
            server.sendToClient(client, server.formatReply(RPL_NOTOPIC, client, channelName, "No topic is set"));
        } else {
            server.sendToClient(client, server.formatReply(RPL_TOPIC, client, channelName, channel->getTopic()));
        }
        return;
    }

    // Setting topic - check if +t mode is set and user is not operator
    if (channel->hasMode('t') && !channel->isOperator(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_CHANOPRIVSNEEDED, client, channelName, "You're not channel operator"));
        return;
    }

    // Set new topic
    std::string newTopic = message.getTrailing();
    channel->setTopic(newTopic);

    // Save topic to persistence (Phase 2 feature)
    server.saveChannelTopic(channelName, newTopic);

    // Broadcast topic change to all members
    Message topicMsg;
    topicMsg.setPrefix(client.getPrefix());
    topicMsg.setCommand("TOPIC");
    topicMsg.addParam(channelName);
    topicMsg.setTrailing(newTopic);
    server.broadcastToChannel(channelName, topicMsg.toString());
}

// MODE command - set channel or user modes
void ModeCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NEEDMOREPARAMS, client, "MODE", "Not enough parameters"));
        return;
    }

    std::string target = message.getParam(0);

    // Check if target is a channel or user
    if (!target.empty() && StringUtils::isValidChannelChar(target[0], true)) {
        // Channel mode
        Channel* channel = server.getChannel(target);
        if (!channel) {
            server.sendToClient(client, server.formatReply(ERR_NOSUCHCHANNEL, client, target, "No such channel"));
            return;
        }
        handleChannelMode(server, client, message, channel);
    } else {
        // User mode
        handleUserMode(server, client, message);
    }
}

void ModeCommand::handleChannelMode(Server& server, Client& client, const Message& message, Channel* channel) {
    std::string channelName = channel->getName();

    // If no mode string, display current modes
    if (message.getParamCount() < 2) {
        std::ostringstream modes;
        modes << "+";
        if (channel->hasMode('i')) modes << "i";
        if (channel->hasMode('t')) modes << "t";
        if (channel->hasMode('k')) modes << "k";
        if (channel->hasMode('l')) modes << "l";

        std::string modeStr = modes.str();
        if (modeStr == "+") modeStr = "+";

        server.sendToClient(client, server.formatReply(RPL_CHANNELMODEIS, client, channelName + " " + modeStr, ""));
        return;
    }

    // Check if user is operator
    if (!channel->isOperator(client.getFd())) {
        server.sendToClient(client, server.formatReply(ERR_CHANOPRIVSNEEDED, client, channelName, "You're not channel operator"));
        return;
    }

    std::string modeString = message.getParam(1);
    bool adding = true;
    size_t paramIndex = 2;
    std::ostringstream appliedModes;
    std::ostringstream appliedParams;

    for (size_t i = 0; i < modeString.length(); ++i) {
        char c = modeString[i];

        if (c == '+') {
            adding = true;
            continue;
        } else if (c == '-') {
            adding = false;
            continue;
        }

        switch (c) {
            case 'i': // Invite-only
                channel->setMode('i', adding);
                appliedModes << (adding ? "+" : "-") << "i";
                break;

            case 't': // Topic protection
                channel->setMode('t', adding);
                appliedModes << (adding ? "+" : "-") << "t";
                break;

            case 'k': // Channel key
                if (adding) {
                    if (paramIndex < message.getParamCount()) {
                        std::string key = message.getParam(paramIndex++);
                        channel->setKey(key);
                        channel->setMode('k', true);
                        appliedModes << "+k";
                        appliedParams << " " << key;
                    }
                } else {
                    channel->setKey("");
                    channel->setMode('k', false);
                    appliedModes << "-k";
                }
                break;

            case 'l': // User limit
                if (adding) {
                    if (paramIndex < message.getParamCount()) {
                        std::string limitStr = message.getParam(paramIndex++);
                        size_t limit = 0;
                        std::istringstream(limitStr) >> limit;
                        channel->setUserLimit(limit);
                        channel->setMode('l', true);
                        appliedModes << "+l";
                        appliedParams << " " << limitStr;
                    }
                } else {
                    channel->setUserLimit(0);
                    channel->setMode('l', false);
                    appliedModes << "-l";
                }
                break;

            case 'o': // Operator status
                if (paramIndex < message.getParamCount()) {
                    std::string targetNick = message.getParam(paramIndex++);
                    Client* target = server.getClientByNick(targetNick);
                    if (target && channel->hasMember(target->getFd())) {
                        if (adding) {
                            channel->addOperator(target->getFd());
                        } else {
                            channel->removeOperator(target->getFd());
                        }
                        appliedModes << (adding ? "+" : "-") << "o";
                        appliedParams << " " << targetNick;
                    }
                }
                break;

            default:
                server.sendToClient(client, server.formatReply(ERR_UNKNOWNMODE, client, std::string(1, c), "is unknown mode char to me"));
                break;
        }
    }

    // Broadcast mode change to channel
    if (!appliedModes.str().empty()) {
        Message modeMsg;
        modeMsg.setPrefix(client.getPrefix());
        modeMsg.setCommand("MODE");
        modeMsg.addParam(channelName);
        modeMsg.addParam(appliedModes.str());
        if (!appliedParams.str().empty()) {
            // Split params and add them individually
            std::string params = appliedParams.str();
            std::istringstream iss(params);
            std::string param;
            while (iss >> param) {
                modeMsg.addParam(param);
            }
        }
        server.broadcastToChannel(channelName, modeMsg.toString());
    }
}

void ModeCommand::handleUserMode(Server& server, Client& client, const Message& message) {
    std::string targetNick = message.getParam(0);

    // Users can only change their own modes
    if (targetNick != client.getNickname()) {
        server.sendToClient(client, server.formatReply(ERR_USERSDONTMATCH, client, "Cannot change mode for other users"));
        return;
    }

    // If no mode string, display current modes
    if (message.getParamCount() < 2) {
        std::string modes = client.getUserModes();
        if (modes.empty()) modes = "+";
        server.sendToClient(client, server.formatReply(RPL_UMODEIS, client, modes, ""));
        return;
    }

    std::string modeString = message.getParam(1);
    bool adding = true;
    std::ostringstream appliedModes;

    for (size_t i = 0; i < modeString.length(); ++i) {
        char c = modeString[i];

        if (c == '+') {
            adding = true;
            continue;
        } else if (c == '-') {
            adding = false;
            continue;
        }

        switch (c) {
            case 'i': // Invisible
                client.setUserMode('i', adding);
                appliedModes << (adding ? "+" : "-") << "i";
                break;

            case 'o': // Operator (can only be removed, not added via MODE)
                if (!adding && client.isOper()) {
                    client.setOperator(false);
                    appliedModes << "-o";
                } else if (adding) {
                    // Cannot set +o via MODE command
                    server.sendToClient(client, server.formatReply(ERR_NOPRIVILEGES, client, "Permission Denied- You're not an IRC operator"));
                }
                break;

            default:
                server.sendToClient(client, server.formatReply(ERR_UMODEUNKNOWNFLAG, client, "Unknown MODE flag"));
                break;
        }
    }

    // Send confirmation
    if (!appliedModes.str().empty()) {
        Message modeMsg;
        modeMsg.setPrefix(client.getPrefix());
        modeMsg.setCommand("MODE");
        modeMsg.addParam(client.getNickname());
        modeMsg.addParam(appliedModes.str());
        server.sendToClient(client, modeMsg.toString());
    }
}

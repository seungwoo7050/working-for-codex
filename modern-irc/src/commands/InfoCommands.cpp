#include "InfoCommands.hpp"
#include "../core/Server.hpp"
#include "../core/Client.hpp"
#include "../core/Channel.hpp"
#include "../core/Message.hpp"
#include <sstream>

// WHOIS command - get information about a user
void WhoisCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    if (message.getParamCount() < 1) {
        server.sendToClient(client, server.formatReply(ERR_NONICKNAMEGIVEN, client, "No nickname given"));
        return;
    }

    std::string targetNick = message.getParam(0);
    Client* target = server.getClientByNick(targetNick);

    if (!target) {
        server.sendToClient(client, server.formatReply(ERR_NOSUCHNICK, client, targetNick, "No such nick/channel"));
        return;
    }

    // RPL_WHOISUSER (311): <nick> <user> <host> * :<real name>
    std::ostringstream userInfo;
    userInfo << target->getNickname() << " " << target->getUsername() << " "
             << target->getHostname() << " *";
    server.sendToClient(client, server.formatReply(RPL_WHOISUSER, client, userInfo.str(), target->getRealname()));

    // RPL_WHOISCHANNELS (319): <nick> :<channels>
    const auto& channels = target->getChannels();
    if (!channels.empty()) {
        std::ostringstream channelList;
        for (const auto& channelName : channels) {
            auto* channel = server.getChannel(channelName);
            if (channel) {
                // Show @ prefix for operators
                if (channel->isOperator(target->getFd())) {
                    channelList << "@";
                }
                channelList << channelName << " ";
            }
        }
        if (!channelList.str().empty()) {
            server.sendToClient(client, server.formatReply(RPL_WHOISCHANNELS, client, target->getNickname(), channelList.str()));
        }
    }

    // RPL_WHOISSERVER (312): <nick> <server> :<server info>
    server.sendToClient(client, server.formatReply(RPL_WHOISSERVER, client,
        target->getNickname() + " " + server.getServerName(), "modern-irc"));

    // RPL_WHOISOPERATOR (313): <nick> :is an IRC operator (if applicable)
    if (target->isOper()) {
        server.sendToClient(client, server.formatReply(RPL_WHOISOPERATOR, client, target->getNickname(), "is an IRC operator"));
    }

    // RPL_AWAY (301): <nick> :<away message> (if away)
    if (target->isAway()) {
        server.sendToClient(client, server.formatReply(RPL_AWAY, client, target->getNickname(), target->getAwayMessage()));
    }

    // RPL_ENDOFWHOIS (318): <nick> :End of /WHOIS list
    server.sendToClient(client, server.formatReply(RPL_ENDOFWHOIS, client, targetNick, "End of /WHOIS list"));
}

// WHO command - list users matching a pattern
void WhoCommand::execute(Server& server, Client& client, const Message& message) {
    if (!client.isRegistered()) {
        return;
    }

    std::string mask = "*";
    if (message.getParamCount() >= 1) {
        mask = message.getParam(0);
    }

    // Check if mask is a channel
    auto* channel = server.getChannel(mask);
    if (channel) {
        // List all members of the channel
        const auto& members = channel->getMembers();
        for (int memberFd : members) {
            auto* member = server.getClient(memberFd);
            if (member) {
                // Skip invisible users unless requester is on the same channel
                if (member->hasUserMode('i') && !channel->hasMember(client.getFd())) {
                    continue;
                }

                // RPL_WHOREPLY (352): <channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>
                std::ostringstream whoReply;
                whoReply << mask << " " << member->getUsername() << " " << member->getHostname()
                         << " " << server.getServerName() << " " << member->getNickname() << " ";

                // H = here, G = gone (away)
                whoReply << (member->isAway() ? "G" : "H");

                // * = IRC operator
                if (member->isOper()) {
                    whoReply << "*";
                }

                // @ = channel operator, + = voiced (we don't have voice, so just op)
                if (channel->isOperator(member->getFd())) {
                    whoReply << "@";
                }

                server.sendToClient(client, server.formatReply(RPL_WHOREPLY, client, whoReply.str(), "0 " + member->getRealname()));
            }
        }
    }

    // RPL_ENDOFWHO (315): <name> :End of /WHO list
    server.sendToClient(client, server.formatReply(RPL_ENDOFWHO, client, mask, "End of /WHO list"));
}

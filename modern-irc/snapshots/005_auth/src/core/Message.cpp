#include "Message.hpp"
#include "../utils/StringUtils.hpp"
#include <sstream>

Message::Message() {
}

Message::Message(const std::string& rawMessage) {
    parse(rawMessage);
}

bool Message::parse(const std::string& rawMessage) {
    // Clear previous state
    prefix.clear();
    command.clear();
    params.clear();
    trailing.clear();

    std::string msg = StringUtils::removeLineEnding(rawMessage);
    if (msg.empty()) {
        return false;
    }

    size_t pos = 0;

    // Parse prefix (optional, starts with :)
    if (msg[0] == ':') {
        size_t spacePos = msg.find(' ', 1);
        if (spacePos == std::string::npos) {
            return false;
        }
        prefix = msg.substr(1, spacePos - 1);
        pos = spacePos + 1;
    }

    // Skip spaces
    while (pos < msg.length() && msg[pos] == ' ') {
        pos++;
    }

    // Parse command
    size_t cmdEnd = msg.find(' ', pos);
    if (cmdEnd == std::string::npos) {
        command = msg.substr(pos);
        return !command.empty();
    }

    command = msg.substr(pos, cmdEnd - pos);
    pos = cmdEnd + 1;

    // Parse parameters
    while (pos < msg.length()) {
        // Skip spaces
        while (pos < msg.length() && msg[pos] == ' ') {
            pos++;
        }

        if (pos >= msg.length()) {
            break;
        }

        // Check for trailing parameter (starts with :)
        if (msg[pos] == ':') {
            trailing = msg.substr(pos + 1);
            break;
        }

        // Parse regular parameter
        size_t paramEnd = msg.find(' ', pos);
        if (paramEnd == std::string::npos) {
            params.push_back(msg.substr(pos));
            break;
        }

        params.push_back(msg.substr(pos, paramEnd - pos));
        pos = paramEnd + 1;
    }

    return !command.empty();
}

std::string Message::toString() const {
    std::ostringstream oss;

    if (!prefix.empty()) {
        oss << ":" << prefix << " ";
    }

    oss << command;

    for (size_t i = 0; i < params.size(); ++i) {
        oss << " " << params[i];
    }

    if (!trailing.empty()) {
        oss << " :" << trailing;
    }

    oss << "\r\n";
    return oss.str();
}

const std::string& Message::getPrefix() const {
    return prefix;
}

const std::string& Message::getCommand() const {
    return command;
}

const std::vector<std::string>& Message::getParams() const {
    return params;
}

const std::string& Message::getTrailing() const {
    return trailing;
}

size_t Message::getParamCount() const {
    return params.size();
}

std::string Message::getParam(size_t index) const {
    if (index < params.size()) {
        return params[index];
    }
    return "";
}

void Message::setPrefix(const std::string& p) {
    prefix = p;
}

void Message::setCommand(const std::string& cmd) {
    command = cmd;
}

void Message::addParam(const std::string& param) {
    params.push_back(param);
}

void Message::setTrailing(const std::string& trail) {
    trailing = trail;
}

bool Message::isValid() const {
    return !command.empty();
}

#include "StringUtils.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string StringUtils::trim(const std::string& str) {
    size_t start = 0, end = str.length();
    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    return str.substr(start, end - start);
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() &&
           str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string StringUtils::removeLineEnding(const std::string& str) {
    std::string result = str;
    if (!result.empty() && result.back() == '\n') result.pop_back();
    if (!result.empty() && result.back() == '\r') result.pop_back();
    return result;
}

bool StringUtils::isValidNickChar(char c, bool first) {
    if (first) {
        return std::isalpha(c) || c == '[' || c == ']' || c == '\\' ||
               c == '`' || c == '_' || c == '^' || c == '{' || c == '|' || c == '}';
    }
    return std::isalnum(c) || c == '[' || c == ']' || c == '\\' ||
           c == '`' || c == '_' || c == '^' || c == '{' || c == '|' || c == '}' || c == '-';
}

bool StringUtils::isValidChannelChar(char c, bool first) {
    if (first) {
        return c == '#' || c == '&';
    }
    return c != ' ' && c != '\x07' && c != ',' && c != '\r' && c != '\n';
}

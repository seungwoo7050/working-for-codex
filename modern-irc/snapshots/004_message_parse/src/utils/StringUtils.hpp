#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>
#include <vector>

class StringUtils {
public:
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);

    // Trim whitespace from both ends
    static std::string trim(const std::string& str);

    // Convert to uppercase
    static std::string toUpper(const std::string& str);

    // Convert to lowercase
    static std::string toLower(const std::string& str);

    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix);

    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix);

    // Remove \r\n from end
    static std::string removeLineEnding(const std::string& str);

    // Check if character is valid for IRC nickname
    static bool isValidNickChar(char c, bool first = false);

    // Check if character is valid for IRC channel name
    static bool isValidChannelChar(char c, bool first = false);
};

#endif

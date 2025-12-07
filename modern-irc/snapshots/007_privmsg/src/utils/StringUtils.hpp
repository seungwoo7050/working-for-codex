#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <string>
#include <vector>

class StringUtils {
public:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string removeLineEnding(const std::string& str);
    static bool isValidNickChar(char c, bool first = false);
    static bool isValidChannelChar(char c, bool first = false);
};

#endif

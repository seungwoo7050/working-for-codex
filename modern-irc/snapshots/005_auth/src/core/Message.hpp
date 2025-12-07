#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

// IRC message format: [:prefix] <command> [params] [:trailing]
class Message {
private:
    std::string prefix;       // Optional sender prefix
    std::string command;      // Command or numeric reply
    std::vector<std::string> params;  // Command parameters
    std::string trailing;     // Trailing parameter (after :)

public:
    Message();
    Message(const std::string& rawMessage);

    // Parse raw IRC message
    bool parse(const std::string& rawMessage);

    // Create formatted IRC message
    std::string toString() const;

    // Getters
    const std::string& getPrefix() const;
    const std::string& getCommand() const;
    const std::vector<std::string>& getParams() const;
    const std::string& getTrailing() const;
    size_t getParamCount() const;
    std::string getParam(size_t index) const;

    // Setters
    void setPrefix(const std::string& prefix);
    void setCommand(const std::string& command);
    void addParam(const std::string& param);
    void setTrailing(const std::string& trailing);

    // Helpers
    bool isValid() const;
};

#endif

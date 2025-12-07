#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>

class Message {
private:
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;

public:
    Message();
    Message(const std::string& rawMessage);

    bool parse(const std::string& rawMessage);
    std::string toString() const;

    const std::string& getPrefix() const;
    const std::string& getCommand() const;
    const std::vector<std::string>& getParams() const;
    const std::string& getTrailing() const;
    size_t getParamCount() const;
    std::string getParam(size_t index) const;

    void setPrefix(const std::string& prefix);
    void setCommand(const std::string& command);
    void addParam(const std::string& param);
    void setTrailing(const std::string& trailing);

    bool isValid() const;
};

#endif

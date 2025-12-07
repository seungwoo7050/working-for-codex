#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>

class Config {
private:
    std::map<std::string, std::string> settings;
    std::string configFilePath;

    Config();
    std::string trim(const std::string& str) const;
    bool parseLine(const std::string& line, std::string& key, std::string& value) const;

public:
    ~Config();

    // Non-copyable, non-movable
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;

    // Singleton access (thread-safe in C++11+)
    static Config& getInstance();

    // File operations
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath);
    bool reload();

    // Getters
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;

    // Setters
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setBool(const std::string& key, bool value);

    // Utilities
    bool hasKey(const std::string& key) const;
    void clear();
    void printAll() const;

    // Get config file path
    std::string getConfigFilePath() const;
};

#endif

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

Config& Config::getInstance() {
    static Config instance;  // C++11+ guarantees thread-safe initialization
    return instance;
}

Config::Config() {
}

Config::~Config() {
}

std::string Config::trim(const std::string& str) const {
    size_t start = 0;
    size_t end = str.length();

    while (start < end && (str[start] == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n')) {
        start++;
    }

    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t' || str[end - 1] == '\r' || str[end - 1] == '\n')) {
        end--;
    }

    return str.substr(start, end - start);
}

bool Config::parseLine(const std::string& line, std::string& key, std::string& value) const {
    std::string trimmedLine = trim(line);

    // Skip empty lines and comments
    if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == ';') {
        return false;
    }

    // Find '=' separator
    size_t equalPos = trimmedLine.find('=');
    if (equalPos == std::string::npos) {
        return false;
    }

    key = trim(trimmedLine.substr(0, equalPos));
    value = trim(trimmedLine.substr(equalPos + 1));

    // Remove quotes from value if present
    if (value.length() >= 2 && value[0] == '"' && value[value.length() - 1] == '"') {
        value = value.substr(1, value.length() - 2);
    }

    return !key.empty();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath.c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filepath << std::endl;
        return false;
    }

    configFilePath = filepath;
    settings.clear();

    std::string line;

    while (std::getline(file, line)) {
        std::string key, value;

        if (parseLine(line, key, value)) {
            settings[key] = value;
        }
    }

    file.close();
    std::cout << "Loaded " << settings.size() << " configuration settings from " << filepath << std::endl;
    return true;
}

bool Config::saveToFile(const std::string& filepath) {
    std::ofstream file(filepath.c_str());
    if (!file.is_open()) {
        std::cerr << "Failed to save config file: " << filepath << std::endl;
        return false;
    }

    file << "# modern-irc Configuration File\n";
    file << "# Auto-generated\n\n";

    for (std::map<std::string, std::string>::const_iterator it = settings.begin(); it != settings.end(); ++it) {
        file << it->first << " = " << it->second << "\n";
    }

    file.close();
    return true;
}

bool Config::reload() {
    if (configFilePath.empty()) {
        std::cerr << "No config file path set" << std::endl;
        return false;
    }
    return loadFromFile(configFilePath);
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    std::map<std::string, std::string>::const_iterator it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) const {
    std::map<std::string, std::string>::const_iterator it = settings.find(key);
    if (it != settings.end()) {
        return atoi(it->second.c_str());
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    std::map<std::string, std::string>::const_iterator it = settings.find(key);
    if (it != settings.end()) {
        std::string value = it->second;
        if (value == "true" || value == "1" || value == "yes" || value == "on") {
            return true;
        }
        if (value == "false" || value == "0" || value == "no" || value == "off") {
            return false;
        }
    }
    return defaultValue;
}

void Config::setString(const std::string& key, const std::string& value) {
    settings[key] = value;
}

void Config::setInt(const std::string& key, int value) {
    std::ostringstream oss;
    oss << value;
    settings[key] = oss.str();
}

void Config::setBool(const std::string& key, bool value) {
    settings[key] = value ? "true" : "false";
}

bool Config::hasKey(const std::string& key) const {
    return settings.find(key) != settings.end();
}

void Config::clear() {
    settings.clear();
}

void Config::printAll() const {
    std::cout << "=== Configuration Settings ===" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = settings.begin(); it != settings.end(); ++it) {
        std::cout << it->first << " = " << it->second << std::endl;
    }
    std::cout << "=============================" << std::endl;
}

std::string Config::getConfigFilePath() const {
    return configFilePath;
}

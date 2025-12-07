#include "TopicPersistence.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

std::string TopicPersistence::getTopicFilePath() {
    return "data/topics.txt";
}

void TopicPersistence::saveTopic(const std::string& channelName, const std::string& topic) {
    // Load all topics
    std::map<std::string, std::string> topics = loadAllTopics();

    // Update the topic for this channel
    if (topic.empty()) {
        topics.erase(channelName);
    } else {
        topics[channelName] = topic;
    }

    // Save all topics
    saveAllTopics(topics);
}

std::string TopicPersistence::loadTopic(const std::string& channelName) {
    auto topics = loadAllTopics();
    if (auto it = topics.find(channelName); it != topics.end()) {
        return it->second;
    }
    return "";
}

void TopicPersistence::deleteTopic(const std::string& channelName) {
    saveTopic(channelName, "");
}

void TopicPersistence::saveAllTopics(const std::map<std::string, std::string>& topics) {
    // Create data directory if it doesn't exist
    mkdir("data", 0755);

    std::ofstream file(getTopicFilePath().c_str());
    if (!file.is_open()) {
        return;  // Silently fail if we can't save
    }

    for (const auto& [channelName, topic] : topics) {
        // Format: channelname|topic
        file << channelName << "|" << topic << "\n";
    }

    file.close();
}

std::map<std::string, std::string> TopicPersistence::loadAllTopics() {
    std::map<std::string, std::string> topics;

    std::ifstream file(getTopicFilePath().c_str());
    if (!file.is_open()) {
        return topics;  // Return empty map if file doesn't exist
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('|');
        if (pos != std::string::npos) {
            std::string channelName = line.substr(0, pos);
            std::string topic = line.substr(pos + 1);
            topics[channelName] = topic;
        }
    }

    file.close();
    return topics;
}

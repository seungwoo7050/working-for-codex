#ifndef TOPICPERSISTENCE_HPP
#define TOPICPERSISTENCE_HPP

#include <string>
#include <map>

class TopicPersistence {
public:
    static void saveTopic(const std::string& channelName, const std::string& topic);
    static std::string loadTopic(const std::string& channelName);
    static void deleteTopic(const std::string& channelName);
    static void saveAllTopics(const std::map<std::string, std::string>& topics);
    static std::map<std::string, std::string> loadAllTopics();

private:
    static std::string getTopicFilePath();
};

#endif

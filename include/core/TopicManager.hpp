#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

class TopicManager {
public:
    bool createTopic(const std::string& topic, const std::string& user);
    bool deleteTopic(const std::string& topic, const std::string& user);
    std::vector<std::string> listTopics();
    bool publishMessage(const std::string& topic, const std::string& message);
    std::vector<std::string> consumeMessages(const std::string& topic);

private:
    std::unordered_map<std::string, std::string> topicOwners;
    std::unordered_map<std::string, std::vector<std::string>> topicMessages;
    std::mutex mutex_;
};

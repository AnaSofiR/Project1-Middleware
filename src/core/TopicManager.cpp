#include "../../include/core/TopicManager.hpp"

bool TopicManager::createTopic(const std::string &topic,
                               const std::string &user) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (topicOwners.count(topic))
    return false;
  topicOwners[topic] = user;
  return true;
}

bool TopicManager::deleteTopic(const std::string &topic,
                               const std::string &user) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (topicOwners.count(topic) == 0 || topicOwners[topic] != user)
    return false;
  topicOwners.erase(topic);
  topicMessages.erase(topic);
  return true;
}

std::vector<std::string> TopicManager::listTopics() {
  std::lock_guard<std::mutex> lock(mutex_);
  std::vector<std::string> result;
  for (const auto &kv : topicOwners) {
    result.push_back(kv.first);
  }
  return result;
}

bool TopicManager::publishMessage(const std::string &topic,
                                  const std::string &message) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (topicMessages.count(topic) == 0)
    return false;
  topicMessages[topic].push_back(message);
  return true;
}

std::vector<std::string>
TopicManager::consumeMessages(const std::string &topic) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (topicMessages.count(topic) == 0)
    return {};
  return topicMessages[topic];
}

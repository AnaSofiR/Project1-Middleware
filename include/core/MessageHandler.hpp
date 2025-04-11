#pragma once
#include <string>
#include "TopicManager.hpp"
#include "QueueManager.hpp"
#include "cluster/ReplicationManager.hpp"

class MessageHandler {
public:
    MessageHandler(TopicManager& topicManager, QueueManager& queueManager,  ReplicationManager& replicationManager);

    void sendToTopic(const std::string& topic, const std::string& message, const std::string& user);
    void sendToQueue(const std::string& queue, const std::string& message, const std::string& user);

    std::vector<std::string> receiveFromTopic(const std::string& topic, const std::string& user);
    std::string receiveFromQueue(const std::string& queue, const std::string& user);

private:
    TopicManager& topicManager;
    QueueManager& queueManager;
    ReplicationManager& replicationManager;
};


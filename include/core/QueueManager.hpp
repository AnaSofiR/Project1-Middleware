#pragma once
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <mutex>
#include "../../build/generated/replication.pb.h"


class QueueManager {
public:
    bool createQueue(const std::string& queue, const std::string& user);
    bool deleteQueue(const std::string& queue, const std::string& user);
    std::vector<std::string> listQueues();
    bool enqueueMessage(const std::string& queue, const std::string& message);
    std::vector<std::string> getMessages(const std::string& queue);
    std::string dequeueMessage(const std::string& queue);
    void applyState(const replication::SystemState& state);

private:
    std::unordered_map<std::string, std::string> queueOwners;
    std::unordered_map<std::string, std::queue<std::string>> queueMessages;
    std::mutex mutex_;
};

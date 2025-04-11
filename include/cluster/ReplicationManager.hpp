#pragma once
#include <string>
#include <vector>

class ReplicationManager {
public:
    ReplicationManager(const std::vector<std::string>& peers);

    void replicateTopic(const std::string& topic, const std::string& message);
    void replicateQueue(const std::string& queue, const std::string& message);

private:
    std::vector<std::string> peerAddresses;

    void sendToPeer(const std::string& address, const std::string& topicOrQueue, const std::string& message, bool isTopic);
};

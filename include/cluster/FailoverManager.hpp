#pragma once
#include <string>
#include <vector>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <memory>
#include <mutex>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>
#include "../../build/generated/replication.grpc.pb.h"
#include "../../build/generated/replication.pb.h"
#include "../core/TopicManager.hpp"
#include "../core/QueueManager.hpp"

class FailoverManager {
public:
    FailoverManager(const std::string& selfAddress, const std::vector<std::string>& peers);
    void startMonitoring();
    void stopMonitoring();

    void syncRecoveredPeer(const std::string& peer);

    void setTopicManager(std::shared_ptr<TopicManager> topicMgr);
    void setQueueManager(std::shared_ptr<QueueManager> queueMgr);

private:
    std::string selfAddress;
    std::vector<std::string> peerAddresses;
    std::unordered_map<std::string, int> failureCounts;
    std::unordered_map<std::string, bool> peerStatus;
    std::string currentLeader;
    std::mutex statusMutex;

    std::atomic<bool> running;
    void monitorLoop();
    bool pingPeer(const std::string& address);

    std::shared_ptr<TopicManager> topicManager_;   
    std::shared_ptr<QueueManager> queueManager_;

    std::string calculateLeader();
    void onLeaderChange(const std::string& newLeader);
};

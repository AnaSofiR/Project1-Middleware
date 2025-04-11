#pragma once
#include <string>
#include <vector>
#include <thread>
#include <unordered_map>
#include <atomic>

class FailoverManager {
public:
    FailoverManager(const std::vector<std::string>& peers);
    void startMonitoring();

private:
    std::vector<std::string> peerAddresses;
    std::unordered_map<std::string, int> failureCounts;
    std::unordered_map<std::string, bool> peerStatus;

    std::atomic<bool> running;
    void monitorLoop();
    bool pingPeer(const std::string& address);
};

#include "../../include/cluster/FailoverManager.hpp"
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

FailoverManager::FailoverManager(const std::string& self, const std::vector<std::string>& peers)
    : selfAddress(self), peerAddresses(peers), running(true) {

    peerStatus[selfAddress] = true;
    failureCounts[selfAddress] = 0;

    for (const auto &addr : peerAddresses) {
        failureCounts[addr] = 0;
        peerStatus[addr] = true;
    }

    currentLeader = calculateLeader();
    std::cout << "[Failover] Líder inicial: " << currentLeader << std::endl;
}

void FailoverManager::startMonitoring() {
    std::thread monitor(&FailoverManager::monitorLoop, this);
    monitor.detach(); 
}

void FailoverManager::stopMonitoring() {
    running = false;
}

void FailoverManager::monitorLoop() {
    const int retryThreshold = 3;

    while (running) {
        for (const auto& addr : peerAddresses) {
            bool alive = pingPeer(addr);

            std::lock_guard<std::mutex> lock(statusMutex);

            if (!alive) {
                failureCounts[addr]++;
                if (peerStatus[addr] && failureCounts[addr] >= retryThreshold) {
                    peerStatus[addr] = false;
                    std::cerr << "[Failover] Nodo marcado como caído: " << addr << std::endl;
                }
            } else {
                if (!peerStatus[addr]) {
                    std::cout << "[Failover] Nodo recuperado: " << addr << std::endl;
                    syncRecoveredPeer(addr);
                }
                failureCounts[addr] = 0;
                peerStatus[addr] = true;
            }
        }

        std::string newLeader = calculateLeader();
        if (newLeader != currentLeader) {
            currentLeader = newLeader;
            onLeaderChange(currentLeader);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

bool FailoverManager::pingPeer(const std::string &address) {
    std::cout << "[Failover] 🔄 Intentando conectar con " << address << std::endl;
    auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());

    auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(3);
    bool connected = channel->WaitForConnected(deadline);

    if (connected) {
        std::cout << "[Failover] ✅ Conectado con " << address << std::endl;
    } else {
        std::cerr << "[Failover] ❌ No se pudo conectar con " << address << std::endl;
    }

    return connected;
}

std::string FailoverManager::calculateLeader() {
    std::string leader;

    for (const auto& [addr, alive] : peerStatus) {
        if (alive && (leader.empty() || addr < leader)) {
            leader = addr;
        }
    }

    std::cout << "[Failover] 🔍 Candidato a líder: " << (leader.empty() ? "Ninguno" : leader) << std::endl;
    return leader;
}

void FailoverManager::onLeaderChange(const std::string& newLeader) {
    std::cout << "[Failover] 👑 Nuevo líder elegido: " << newLeader << std::endl;
}

void FailoverManager::setTopicManager(std::shared_ptr<TopicManager> topicMgr) {
    topicManager_ = topicMgr;
}

void FailoverManager::setQueueManager(std::shared_ptr<QueueManager> queueMgr) {
    queueManager_ = queueMgr;
}

void FailoverManager::syncRecoveredPeer(const std::string& peer) {
    std::cout << "[SYNC] 🔄 Intentando sincronizar con " << peer << std::endl;

    auto channel = grpc::CreateChannel(peer, grpc::InsecureChannelCredentials());
    auto stub = replication::ReplicationService::NewStub(channel);

    grpc::ClientContext context;
    google::protobuf::Empty request;
    replication::SystemState response;

    grpc::Status status = stub->SyncState(&context, request, &response);
    if (status.ok()) {
        if (response.topicmessages_size() == 0 && response.queuemessages_size() == 0){
            std::cout << "[SYNC] No hay datos que sincornizar desde" << peer << std::endl;
        } else {
            if (topicManager_) topicManager_->applyState(response);
            if (queueManager_) queueManager_->applyState(response);
            std::cout << "[SYNC] ✅ Nodo sincronizado desde " << peer << std::endl;
        }
    } else {
        std::cerr << "[SYNC] ❌ Error al sincronizar con " << peer
                  << ": " << status.error_message() << std::endl;
    }
}

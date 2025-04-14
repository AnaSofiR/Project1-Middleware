#include "../../include/cluster/FailoverManager.hpp"
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <thread>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// Simulamos un ping llamando a un método vacío en un servicio básico.
// Puedes extenderlo para usar un servicio como `HealthCheckService`.

FailoverManager::FailoverManager(const std::vector<std::string> &peers)
    : peerAddresses(peers), running(true) {
  currentLeader = calculateLeader();
  std::cout << "[Failover] Líder inicial: " << currentLeader << std::endl;
  for (const auto &addr : peerAddresses) {
    failureCounts[addr] = 0;
    peerStatus[addr] = true;
  }
}

void FailoverManager::startMonitoring() {
  std::thread monitor(&FailoverManager::monitorLoop, this);
  monitor.detach(); // Corre en segundo plano
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
                    std::cerr << "[Failover] Nodo caído: " << addr << std::endl;
                }
            } else {
                if (!peerStatus[addr]) {
                    std::cout << "[Failover] Nodo recuperado: " << addr << std::endl;
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

// Simulación de ping (a futuro puedes implementar un método real como
// HealthCheck en gRPC)
bool FailoverManager::pingPeer(const std::string &address) {
  auto channel =
      grpc::CreateChannel(address, grpc::InsecureChannelCredentials());

  // Aquí podrías hacer un real gRPC ping, por ahora solo intentamos crear el
  // canal.
  return channel->WaitForConnected(std::chrono::system_clock::now() +
                                   std::chrono::seconds(1));
}


std::string FailoverManager::calculateLeader() {
    std::string leader = "zzzzzz";
    for (const auto& [addr, alive] : peerStatus) {
        if (alive && addr < leader) {
            leader = addr;
        }
    }
    return leader;
}

void FailoverManager::onLeaderChange(const std::string& newLeader) {
    std::cout << "[Failover] Nuevo líder: " << newLeader << std::endl;

    // Aquí podrías notificar a otros componentes (como TopicManager)
    // o hacer una sincronización si eres un nodo que se reincorpora
}

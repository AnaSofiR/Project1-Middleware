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
    for (const auto &addr : peerAddresses) {
      bool alive = pingPeer(addr);

      if (!alive) {
        failureCounts[addr]++;
        if (peerStatus[addr] && failureCounts[addr] >= retryThreshold) {
          peerStatus[addr] = false;
          std::cerr << "[Failover] Nodo caído: " << addr << std::endl;
          // Aquí podrías disparar lógica de redistribución
        }
      } else {
        if (!peerStatus[addr]) {
          std::cout << "[Failover] Nodo recuperado: " << addr << std::endl;
        }
        failureCounts[addr] = 0;
        peerStatus[addr] = true;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Espera entre pings
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

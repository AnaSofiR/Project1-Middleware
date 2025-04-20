#include "../include/cluster/FailoverManager.hpp"
#include "../include/cluster/HostReceiver.hpp"
#include "../include/cluster/NodeAnnouncer.hpp"
#include "../include/cluster/ReplicationManager.hpp"
#include "../include/core/QueueManager.hpp"
#include "../include/core/TopicManager.hpp"
#include "../include/networking/GrpcServer.hpp"
#include "../include/networking/RestServer.hpp"
#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0]
              << " <puerto_grpc> [puerto_rest] [peer1] [peer2] ..."
              << std::endl;
    return 1;
  }

  int grpcPort = std::stoi(argv[1]);
  int restPort = (argc >= 3) ? std::stoi(argv[2]) : grpcPort + 1000;

  std::vector<std::string> peers;
  for (int i = 3; i < argc; ++i) {
    peers.push_back(argv[i]);
  }

  auto topicManager = std::make_shared<TopicManager>();
  auto queueManager = std::make_shared<QueueManager>();
  std::shared_ptr<ReplicationManager> replManager =
      std::make_shared<ReplicationManager>(peers);
  std::string selfAddress = "localhost:" + std::to_string(grpcPort);
  auto failoverManager = std::make_shared<FailoverManager>(selfAddress, peers);

  failoverManager->setTopicManager(topicManager);
  failoverManager->setQueueManager(queueManager);
  failoverManager->startMonitoring();

  std::cout << "[MOM] Iniciando servidor..." << std::endl;
  std::cout << "gRPC escuchando en puerto: " << grpcPort << std::endl;
  std::cout << "REST escuchando en puerto: " << restPort << std::endl;

  RestServer rest;
  rest.setReplicationManager(replManager);

  // 🔄 Ejecutar el servidor REST en un hilo separado
  std::thread restThread([&]() { rest.start(restPort); });

  // 🔄 Ejecutar gRPC en el hilo principal
  GrpcServer grpc;
  grpc.setReplicationManager(replManager, topicManager, queueManager);
  grpc.start(grpcPort);

  // Esperar al hilo del servidor REST
  restThread.join();

  return 0;
}

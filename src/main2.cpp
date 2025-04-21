#include "../include/cluster/FailoverManager.hpp"
#include "../include/cluster/HostReceiver.hpp"
#include "../include/cluster/NodeAnnouncer.hpp"
#include "../include/cluster/ReplicationManager.hpp"
#include "../include/core/QueueManager.hpp"
#include "../include/core/TopicManager.hpp"
#include "../include/networking/GrpcServer.hpp"
#include "../include/networking/RestServer.hpp"
#include <cstring>
#include <iostream>
#include <thread>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0]
              << " <puerto_grpc> [puerto_rest] [peer1] [peer2] ... "
                 "[--host <port>] [--announce <host_ip>]"
              << std::endl;
    return 1;
  }

  int grpcPort = std::stoi(argv[1]);
  int restPort =
      (argc >= 3 && argv[2][0] != '-') ? std::stoi(argv[2]) : grpcPort + 1000;

  std::vector<std::string> peers;
  std::string announceTarget = "";
  int hostListenPort = -1;

  // Procesar argumentos
  for (int i = 3; i < argc; ++i) {
    if (std::strcmp(argv[i], "--announce") == 0 && i + 1 < argc) {
      announceTarget = argv[++i];
    } else if (std::strcmp(argv[i], "--host") == 0 && i + 1 < argc) {
      hostListenPort = std::stoi(argv[++i]);
    } else if (argv[i][0] != '-') {
      peers.push_back(argv[i]);
    }
  }

  auto topicManager = std::make_shared<TopicManager>();
  auto queueManager = std::make_shared<QueueManager>();
  std::shared_ptr<ReplicationManager> replManager =
      std::make_shared<ReplicationManager>(peers);

  std::string selfAddress =
      getLocalIPAddress() + ":" + std::to_string(grpcPort);
  auto failoverManager = std::make_shared<FailoverManager>(selfAddress, peers);

  failoverManager->setTopicManager(topicManager);
  failoverManager->setQueueManager(queueManager);
  failoverManager->startMonitoring();

  if (hostListenPort != -1) {
    std::cout << "[Host] Activando modo host en puerto TCP: " << hostListenPort
              << std::endl;
    runHostListener(*failoverManager, hostListenPort);
  }

  if (!announceTarget.empty()) {
    std::cout << "[Announce] Anunciando este nodo al host: " << announceTarget
              << std::endl;
    announceToHost(announceTarget,
                   hostListenPort != -1 ? hostListenPort : 6000);
  }

  std::cout << "[MOM] Iniciando servidor..." << std::endl;
  std::cout << "gRPC escuchando en puerto: " << grpcPort << std::endl;
  std::cout << "REST escuchando en puerto: " << restPort << std::endl;

  RestServer rest;
  // rest.setReplicationManager(replManager);

  std::thread restThread([&]() { rest.start(restPort); });

  GrpcServer grpc;
  grpc.setReplicationManager(replManager, topicManager, queueManager);
  grpc.start(grpcPort);

  restThread.join();

  return 0;
}

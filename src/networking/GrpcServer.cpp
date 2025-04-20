#include "../../include/networking/GrpcServer.hpp"
#include "../../include/cluster/ReplicationServiceImpl.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>

GrpcServer::GrpcServer() {}

void GrpcServer::setReplicationManager(std::shared_ptr<ReplicationManager> repl,
                                       std::shared_ptr<TopicManager> topic,
                                       std::shared_ptr<QueueManager> queue) {
  replicationManager_ = repl;
  topicManager_ = topic;
  queueManager_ = queue;
}

void GrpcServer::start(int port) {
  std::string address = "0.0.0.0:" + std::to_string(port);
  grpc::ServerBuilder builder;

  builder.AddListeningPort(address, grpc::InsecureServerCredentials());

  // ✅ Crear y guardar la instancia
  replicationService_ = std::make_unique<ReplicationServiceImpl>(
      replicationManager_, topicManager_, queueManager_);
  builder.RegisterService(replicationService_.get());

  std::cout << "[gRPC] Servidor escuchando en " << address << std::endl;

  std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
  server->Wait();
}

#include "../../include/cluster/ReplicationManager.hpp"
#include "../../build/generated/replication.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

ReplicationManager::ReplicationManager(const std::vector<std::string> &peers)
    : peerAddresses(peers) {}

void ReplicationManager::replicateTopic(const std::string &topic,
                                        const std::string &message) {
  for (const auto &peer : peerAddresses) {
    sendToPeer(peer, topic, message, true);
  }
}

void ReplicationManager::replicateQueue(const std::string &queue,
                                        const std::string &message) {
  for (const auto &peer : peerAddresses) {
    sendToPeer(peer, queue, message, false);
  }
}

void ReplicationManager::sendToPeer(const std::string &address,
                                    const std::string &name,
                                    const std::string &message, bool isTopic) {
  auto channel =
      grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
  std::unique_ptr<replication::ReplicationService::Stub> stub =
      replication::ReplicationService::NewStub(channel);

  grpc::ClientContext context;
  replication::Ack reply;

  if (isTopic) {
    replication::TopicMessage req;
    req.set_topic(name);
    req.set_message(message);
    Status status = stub->ReplicateTopicMessage(&context, req, &reply);
    if (status.ok()) {
      std::cout << "[Replication] Tópico '" << name << "' replicado a "
                << address << "\n";
    } else {
      std::cerr << "[Replication] Error replicando tópico a " << address
                << "\n";
    }
  } else {
    replication::QueueMessage req;
    req.set_queue(name);
    req.set_message(message);
    Status status = stub->ReplicateQueueMessage(&context, req, &reply);
    if (status.ok()) {
      std::cout << "[Replication] Cola '" << name << "' replicada a " << address
                << "\n";
    } else {
      std::cerr << "[Replication] Error replicando cola a " << address << "\n";
    }
  }
}



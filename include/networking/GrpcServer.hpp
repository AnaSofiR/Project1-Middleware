#pragma once
#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "../cluster/ReplicationManager.hpp"
#include "../core/TopicManager.hpp"   
#include "../core/QueueManager.hpp"


class GrpcServer {
public:
    GrpcServer();
    void start(int port);
    void setReplicationManager(std::shared_ptr<ReplicationManager> repl,
                                std::shared_ptr<TopicManager> topic,
                                std::shared_ptr<QueueManager> queue);

private:
    std::shared_ptr<ReplicationManager> replicationManager_;
    std::shared_ptr<TopicManager> topicManager_;
    std::shared_ptr<QueueManager> queueManager_;
    std::unique_ptr<grpc::Service> replicationService_;
};

#pragma once

#include "../../build/generated/replication.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <google/protobuf/empty.pb.h>
#include <memory>
#include <mutex>
#include "../cluster/ReplicationManager.hpp"
#include "../core/TopicManager.hpp"
#include "../core/QueueManager.hpp"

class ReplicationServiceImpl : public replication::ReplicationService::Service {
public:
    ReplicationServiceImpl(std::shared_ptr<ReplicationManager> manager,
                           std::shared_ptr<TopicManager> topicManager,
                           std::shared_ptr<QueueManager> queueManager);

    grpc::Status ReplicateTopicMessage(grpc::ServerContext* context,
                                const replication::TopicMessage* request,
                                replication::Ack* response) override;

    grpc::Status ReplicateQueueMessage(grpc::ServerContext* context,
                                       const replication::QueueMessage* request,
                                       replication::Ack* response) override;

    grpc::Status SyncState(grpc::ServerContext* context,
                           const google::protobuf::Empty* request,
                           replication::SystemState* response) override;

private:
    std::shared_ptr<ReplicationManager> replicationManager_;
    std::shared_ptr<TopicManager> topicManager_;
    std::shared_ptr<QueueManager> queueManager_;
    std::mutex syncMutex_;
};

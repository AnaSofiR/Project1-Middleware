// include/cluster/ReplicationServiceImpl.hpp
#pragma once

#include "../../build/generated/replication.grpc.pb.h"
#include <memory>
#include "../cluster/ReplicationManager.hpp"

class ReplicationServiceImpl : public replication::ReplicationService::Service {
public:
    ReplicationServiceImpl(std::shared_ptr<ReplicationManager> manager);

    grpc::Status ReplicateTopic(grpc::ServerContext* context,
                                const replication::TopicMessage* request,
                                replication::Ack* response) override;

    grpc::Status ReplicateQueueMessage(grpc::ServerContext* context,
                                       const replication::QueueMessage* request,
                                       replication::Ack* response) override;

private:
    std::shared_ptr<ReplicationManager> replicationManager_;
};

#include "../../include/cluster/ReplicationServiceImpl.hpp"
#include <iostream>

ReplicationServiceImpl::ReplicationServiceImpl(std::shared_ptr<ReplicationManager> manager)
    : replicationManager_(manager) {}

grpc::Status ReplicationServiceImpl::ReplicateTopic(
    grpc::ServerContext* context,
    const replication::TopicMessage* request,
    replication::Ack* response
) {
    std::cout << "[RECEPCIÓN] Replica recibida del tópico "
              << request->topic() << ": " << request->message() << std::endl;

    response->set_success(true);
    return grpc::Status::OK;
}

grpc::Status ReplicationServiceImpl::ReplicateQueueMessage(
    grpc::ServerContext* context,
    const replication::QueueMessage* request,
    replication::Ack* response
) {
    return grpc::Status::OK;
}

#include "../../include/cluster/ReplicationServiceImpl.hpp"
#include <iostream>

ReplicationServiceImpl::ReplicationServiceImpl(std::shared_ptr<ReplicationManager> manager, std::shared_ptr<TopicManager> topicManager,
std::shared_ptr<QueueManager> queueManager)
    : replicationManager_(manager),
      topicManager_(topicManager),
      queueManager_(queueManager) {}

grpc::Status ReplicationServiceImpl::ReplicateTopicMessage(
    grpc::ServerContext* context,
    const replication::TopicMessage* request,
    replication::Ack* response
) {

    std::string topic = request->topic();
    std::string message = request->message();

    std::cout << "[RECEPCIÓN] Replica recibida del tópico "
              << topic << ": " << message << std::endl;

    if (topicManager_){
        topicManager_->publishMessage(topic, message);
        response->set_success(true);
        return grpc::Status::OK;
    }

    response->set_success(false);
    return grpc::Status(grpc::StatusCode::INTERNAL, "TopicManager no disponible");
}

grpc::Status ReplicationServiceImpl::ReplicateQueueMessage(
    grpc::ServerContext* context,
    const replication::QueueMessage* request,
    replication::Ack* response
) {
    std::string queue = request->queue();
    std::string message = request->message();

    std::cout << "[RECEPCIÓN] Replica recibida de la cola: " << queue << " : " << message << std::endl;

    if (queueManager_) {
        queueManager_->enqueueMessage(queue, message);
        response->set_success(true);
        return grpc::Status::OK;
    }

    response->set_success(false);
    return grpc::Status(grpc::StatusCode::INTERNAL, "QueueManager no disponible");
}

grpc::Status ReplicationServiceImpl::SyncState(
    grpc::ServerContext* context,
    const google::protobuf::Empty* request,
    replication::SystemState* response
) {
    std::lock_guard<std::mutex> lock(syncMutex_);

    for (const auto& topic : topicManager_->listTopics()) {
        response->add_topics(topic);
        auto* topicMsg = response->add_topicmessages();
        topicMsg->set_topic(topic);
        for (const auto& msg : topicManager_->consumeMessages(topic)) {
            topicMsg->add_messages(msg);
        }
    }

    for (const auto& queue : queueManager_->listQueues()) {
        response->add_queues(queue);
        auto* queueMsg = response->add_queuemessages();
        queueMsg->set_queue(queue);
        for (const auto& msg : queueManager_->getMessages(queue)) {
            queueMsg->add_messages(msg);
        }
    }

    return grpc::Status::OK;
}
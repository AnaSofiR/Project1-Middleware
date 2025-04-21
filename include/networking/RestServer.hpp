#pragma once

#include "../auth/AuthService.hpp"
#include "../utils/Config.hpp"
#include "../utils/Logger.hpp"
#include "crow.h"
#include <httplib.h>
#include "../cluster/ReplicationManager.hpp"
#include "../core/QueueManager.hpp"
#include "../core/TopicManager.hpp"
#include "../core/MessageHandler.hpp"


class RestServer {
public:
    RestServer();

    void start(int port);
    void setReplicationManager(std::shared_ptr<ReplicationManager> repl);

private:
    std::shared_ptr<ReplicationManager> replicationManager_;
    std::shared_ptr<TopicManager> topicManager_;
    std::shared_ptr<QueueManager> queueManager_;
    std::shared_ptr<MessageHandler> messageHandler_;
};

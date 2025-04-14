#pragma once
#include <string>
#include "../cluster/ReplicationManager.hpp"


class GrpcServer {
public:
    GrpcServer();
    void start(int port);
    void setReplicationManager(std::shared_ptr<ReplicationManager> repl);

private:
    std::shared_ptr<ReplicationManager> replicationManager_;
};

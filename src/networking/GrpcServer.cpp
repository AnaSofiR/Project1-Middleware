#include "../../include/networking/GrpcServer.hpp"
#include "../../include/cluster/ReplicationServiceImpl.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>

GrpcServer::GrpcServer() {}

void GrpcServer::setReplicationManager(std::shared_ptr<ReplicationManager> repl) {
    replicationManager_ = repl;
}

void GrpcServer::start(int port) {
    std::string address = "0.0.0.0:" + std::to_string(port);
    grpc::ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());

    // ✅ REGISTRA EL SERVICIO GPRC
    auto service = std::make_unique<ReplicationServiceImpl>(replicationManager_);
    builder.RegisterService(service.get());


    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
    
    std::cout << "[gRPC] Servidor escuchando en " << address << std::endl;

    server->Wait();
}

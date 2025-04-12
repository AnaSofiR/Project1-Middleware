#include "../../include/networking/GrpcServer.hpp"
#include <grpcpp/grpcpp.h>
#include <iostream>

using grpc::Server;
using grpc::ServerBuilder;

GrpcServer::GrpcServer() {}

void GrpcServer::start(int port) {
  std::string address = "0.0.0.0:" + std::to_string(port);
  ServerBuilder builder;

  builder.AddListeningPort(address, grpc::InsecureServerCredentials());

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "[gRPC] Servidor escuchando en " << address << std::endl;
  server->Wait();
}

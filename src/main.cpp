#include "../include/networking/GrpcServer.hpp"
#include "../include/networking/RestServer.hpp"
#include <iostream>

int main() {
  std::cout << "[MOM] Iniciando servidor..." << std::endl;

  // Iniciar servicios
  RestServer rest = RestServer();
  rest.start(8080);

  GrpcServer grpc = GrpcServer();
  grpc.start(50051);

  return 0;
}

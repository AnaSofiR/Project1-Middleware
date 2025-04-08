#include <iostream>
#include "networking/RestServer.hpp"
#include "networking/GrpcServer.hpp"

int main() {
    std::cout << "[MOM] Iniciando servidor..." << std::endl;

    // Iniciar servicios
    RestServer rest;
    rest.start(8080);

    GrpcServer grpc;
    grpc.start(50051);

    return 0;
}

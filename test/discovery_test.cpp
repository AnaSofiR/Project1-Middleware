#include "../include/cluster/DiscoveryService.hpp"
#include <iostream>

int main() {
  // Simula un nodo que ya está activo
  startDiscoveryResponder(8888, 8080);

  std::this_thread::sleep_for(std::chrono::seconds(1)); // Espera para iniciar

  // Nodo nuevo descubre a los demás
  auto nodes = discoverClusterNodes(8888);

  for (const auto &node : nodes) {
    std::cout << "Nodo encontrado: " << node.ip << ":" << node.port
              << std::endl;
  }

  return 0;
}

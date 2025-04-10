// descubridor.cpp
#include "../include/cluster/DiscoveryService.hpp"
#include <iostream>

int main() {
  std::cout << "[Discover] Iniciando el descubrimiento en el puerto 8888."
            << std::endl;

  // Envía el mensaje DISCOVER y espera respuestas (timeout de 2 segundos)
  auto nodes = discoverClusterNodes(8888, 2);

  if (nodes.empty()) {
    std::cout << "[Discover] No se encontraron nodos." << std::endl;
  } else {
    for (const auto &node : nodes) {
      std::cout << "Nodo encontrado: " << node.ip << ":" << node.port
                << std::endl;
    }
  }

  return 0;
}

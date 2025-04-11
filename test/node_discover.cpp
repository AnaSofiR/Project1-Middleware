// descubridor.cpp
#include "../include/cluster/DiscoveryService.hpp"
#include <iostream>

int main() {
  std::string subnet_prefix = "172.31"; // Modifica según tu subred
  int listen_port = 9999;

  std::cout << "[Node Discover] Buscando nodos en la subred " << subnet_prefix
            << ".x.x\n";

  auto nodes = discoverClusterNodes(subnet_prefix, listen_port);

  std::cout << "[Node Discover] Nodos encontrados:\n";
  for (const auto &node : nodes) {
    std::cout << "  - " << node.ip << ":" << node.port << "\n";
  }

  return 0;
}

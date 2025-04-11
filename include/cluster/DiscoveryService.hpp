
#ifndef DISCOVERY_HPP
#define DISCOVERY_HPP

#pragma once
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

struct NodeInfo {
  std::string ip;
  int port;
};

// Nodo pasivo que responde al descubridor
void startDiscoveryResponder(int listen_port, int service_port);

// Nodo activo que busca otros
std::vector<NodeInfo>
discoverClusterNodes(const std::string &subnet_prefix, // ej: "172.31.96"
                     int listen_port // puerto donde escuchan los demás
);
#endif

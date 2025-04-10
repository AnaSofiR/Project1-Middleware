
#ifndef DISCOVERY_HPP
#define DISCOVERY_HPP

#include <arpa/inet.h>
#include <cstring>
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

void startDiscoveryResponder(int listenPort, int responsePort);
std::vector<NodeInfo> discoverClusterNodes(int discoveryPort,
                                           int timeoutSecs = 2);

#endif

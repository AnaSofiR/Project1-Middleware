// src/discovery/NodeAnnouncer.cpp
#include "../../include/cluster/NodeAnnouncer.hpp"

void announceToHost(const std::string &hostIp, int port) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Socket error");
    return;
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  inet_pton(AF_INET, hostIp.c_str(), &serverAddr.sin_addr);

  if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("Connect error");
    close(sock);
    return;
  }

  std::string ip = getLocalIPAddress();
  send(sock, ip.c_str(), ip.length(), 0);
  std::cout << "[Announcer] Enviada IP al host: " << ip << std::endl;

  close(sock);
}


#include "../../include/cluster/NodeAnnouncer.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

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
  int grpcPort = 55001; // fijo

  // Obtener hora actual en formato ISO8601
  auto t = std::time(nullptr);
  std::ostringstream timestamp;
  timestamp << std::put_time(std::gmtime(&t), "%FT%TZ");

  std::string message =
      ip + ":" + std::to_string(grpcPort) + "|" + timestamp.str();
  send(sock, message.c_str(), message.length(), 0);

  std::cout << "[Announcer] Nodo anunciado al host: " << message << std::endl;
  close(sock);
}

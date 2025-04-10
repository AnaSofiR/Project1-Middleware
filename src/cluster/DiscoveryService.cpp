#include "../../include/cluster/DiscoveryService.hpp"

void startDiscoveryResponder(int listenPort, int responsePort) {
  std::thread([listenPort, responsePort]() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(listenPort);
    bind(sock, (sockaddr *)&addr, sizeof(addr));

    char buffer[256];
    sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);

    // En startDiscoveryResponder
    std::cout << "[Responder] Esperando mensajes en el puerto " << listenPort
              << std::endl;

    while (true) {
      int len = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (sockaddr *)&senderAddr, &senderLen);
      buffer[len] = '\0';

      std::string msg(buffer);
      if (msg == "DISCOVER") {
        // Cuando recibe algo:
        std::cout << "[Responder] Recibido mensaje de descubrimiento. Enviando "
                     "respuesta desde puerto "
                  << responsePort << std::endl;

        std::string response =
            "I_AM_NODE:127.0.0.1:" + std::to_string(responsePort);
        sendto(sock, response.c_str(), response.size(), 0,
               (sockaddr *)&senderAddr, senderLen);
      }
    }

    close(sock);
  }).detach();
}

std::vector<NodeInfo> discoverClusterNodes(int discoveryPort, int timeoutSecs) {
  std::vector<NodeInfo> nodes;

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  int broadcastEnable = 1;
  setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable,
             sizeof(broadcastEnable));

  sockaddr_in broadcastAddr{};
  broadcastAddr.sin_family = AF_INET;
  broadcastAddr.sin_port = htons(discoveryPort);
  broadcastAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  std::string discoverMsg = "DISCOVER";
  sendto(sock, discoverMsg.c_str(), discoverMsg.size(), 0,
         (sockaddr *)&broadcastAddr, sizeof(broadcastAddr));

  // Set timeout for responses
  timeval timeout{timeoutSecs, 0};
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  sockaddr_in sender{};
  socklen_t senderLen = sizeof(sender);
  char buffer[256];

  std::cout << "[Discover] Enviando mensaje de descubrimiento por puerto "
            << discoveryPort << std::endl;

  while (true) {
    int len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (sockaddr *)&sender,
                       &senderLen);
    if (len < 0)
      break;

    buffer[len] = '\0';
    std::string msg(buffer);

    if (msg.find("I_AM_NODE") == 0) {
      std::string ip = inet_ntoa(sender.sin_addr);
      int port = std::stoi(msg.substr(msg.find_last_of(':') + 1));

      // Después de recibir respuestas
      std::cout << "[Discover] Nodo descubierto: " << ip << ":" << port
                << std::endl;

      nodes.push_back({ip, port});
    }
  }

  close(sock);
  return nodes;
}

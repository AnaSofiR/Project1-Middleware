// src/discovery/HostReceiver.cpp
#include "../../include/cluster/HostReceiver.hpp"


  void runHostListener(FailoverManager & manager, int port) {
    std::thread([&manager, port]() {
      int server_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (server_fd < 0) {
        perror("Socket error");
        return;
      }

      sockaddr_in serverAddr{};
      serverAddr.sin_family = AF_INET;
      serverAddr.sin_addr.s_addr = INADDR_ANY;
      serverAddr.sin_port = htons(port);

      bind(server_fd, (sockaddr *)&serverAddr, sizeof(serverAddr));
      listen(server_fd, 10);
      std::cout << "[HostReceiver] Esperando conexiones TCP en puerto " << port
                << std::endl;

      while (true) {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int client_fd = accept(server_fd, (sockaddr *)&clientAddr, &addrLen);

        if (client_fd >= 0) {
          char buffer[1024] = {0};
          int len = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
          buffer[len] = '\0';

          std::string data(buffer);
          auto sepPos = data.find('|');

          if (sepPos != std::string::npos) {
            std::string address = data.substr(0, sepPos);
            std::string timestamp = data.substr(sepPos + 1);

            std::cout << "[HostReceiver] Nodo recibido: " << address
                      << " (Creado en: " << timestamp << ")" << std::endl;

            manager.addPeerAddress(address);
          } else {
            std::cerr << "[HostReceiver] Formato inválido recibido: " << data
                      << std::endl;
          }

          close(client_fd);
        }
      }

      close(server_fd);
    }).detach();
  }


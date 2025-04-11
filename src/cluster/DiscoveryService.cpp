#include "../../include/cluster/DiscoveryService.hpp"

void startDiscoveryResponder(int listen_port, int service_port) {
  std::thread([=]() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr{}, sender{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(listen_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (sockaddr *)&addr, sizeof(addr));

    char buffer[1024];
    socklen_t len = sizeof(sender);
    while (true) {
      ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                           (sockaddr *)&sender, &len);
      if (n > 0) {
        std::string response = std::to_string(service_port);
        sendto(sockfd, response.c_str(), response.size(), 0,
               (sockaddr *)&sender, len);
      }
    }
    close(sockfd);
  }).detach();
}

std::vector<NodeInfo> discoverClusterNodes(const std::string &subnet_prefix,
                                           int listen_port) {
  std::vector<NodeInfo> nodes;
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct timeval tv = {1, 0}; // 1s timeout
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

  for (int i = 1; i < 255; ++i) {
    for (int j = 0; j < 255; ++j) {
      std::string ip =
          subnet_prefix + "." + std::to_string(i) + "." + std::to_string(j);
      sockaddr_in target{};
      target.sin_family = AF_INET;
      target.sin_port = htons(listen_port);
      inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

      std::string ping = "DISCOVER";
      sendto(sockfd, ping.c_str(), ping.size(), 0, (sockaddr *)&target,
             sizeof(target));

      char buf[100];
      socklen_t len = sizeof(target);
      int n = recvfrom(sockfd, buf, sizeof(buf), 0, (sockaddr *)&target, &len);
      if (n > 0) {
        buf[n] = '\0';
        nodes.push_back({ip, std::stoi(buf)});
      }
    }
  }
  close(sockfd);
  return nodes;
}

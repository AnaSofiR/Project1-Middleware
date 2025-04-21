// utils/NetworkUtils.cpp
#include "../../include/utils/NetworksUtils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <ifaddrs.h>
#include <netinet/in.h>

std::string getLocalIPAddress() {
  struct ifaddrs *interfaces = nullptr;
  getifaddrs(&interfaces);
  std::string ip = "127.0.0.1"; // fallback

  for (auto *ifa = interfaces; ifa != nullptr; ifa = ifa->ifa_next) {
    if (!ifa->ifa_addr)
      continue;
    if (ifa->ifa_addr->sa_family == AF_INET &&
        std::string(ifa->ifa_name).find("lo") == std::string::npos) {
      ip = inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr);
      break;
    }
  }

  freeifaddrs(interfaces);
  return ip;
}

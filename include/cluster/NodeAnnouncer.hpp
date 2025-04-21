#pragma once
#include "../utils/NetworksUtils.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void announceToHost(const std::string &hostIp, int port);

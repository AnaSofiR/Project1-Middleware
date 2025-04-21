#pragma once
#include "FailoverManager.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void runHostListener(FailoverManager &manager, int port);

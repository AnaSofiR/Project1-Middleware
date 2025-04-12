#pragma once
#include <string>

class GrpcServer {
public:
  GrpcServer();

  void start(int port);
};

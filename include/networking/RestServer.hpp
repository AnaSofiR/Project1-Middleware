#pragma once

#include "crow.h"
#include <httplib.h>

class RestServer {
public:
  void start(int port);
};

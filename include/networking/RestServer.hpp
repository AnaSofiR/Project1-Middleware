#pragma once

#include "auth/AuthService.hpp"
#include "../utils/Config.hpp"
#include "../utils/Logger.hpp"
#include "crow.h"
#include <httplib.h>

class RestServer {
public:
  void start(int port);
};

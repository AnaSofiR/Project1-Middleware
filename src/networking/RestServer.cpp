// En RestServer.cpp
#include "../../include/auth/AuthService.hpp"

AuthService auth_service("postgresql://user:password@localhost/momdb");

server RestServer =

    // Endpoint de registro
    server.Post("/register", [&](const Request &req, Response &res) {
      std::string username = req.get_param_value("username");
      std::string password = req.get_param_value("password");

      if (auth_service.registerUser(username, password)) {
        res.set_content("Registro exitoso", "text/plain");
      } else {
        res.status = 400;
        res.set_content("Error en registro", "text/plain");
      }
    });

// Endpoint de login
server.Post("/login", [&](const Request &req, Response &res) {
  std::string username = req.get_param_value("username");
  std::string password = req.get_param_value("password");

  std::string token = auth_service.loginUser(username, password);
  if (!token.empty()) {
    res.set_content(token, "text/plain");
  } else {
    res.status = 401;
    res.set_content("Credenciales inválidas", "text/plain");
  }
});

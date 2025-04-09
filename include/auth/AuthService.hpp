// src/auth/AuthService.hpp

// include/auth/AuthService.hpp
#ifndef AUTHSERVICE_HPP
#define AUTHSERVICE_HPP

#include <jwt-cpp/jwt.h>
#include <memory>
#include <openssl/sha.h>
#include <pqxx/pqxx>
#include <string>
// Incluimos configuraciones y logging del proyecto
#include "../utils/Config.hpp"
#include "../utils/Logger.hpp"

class AuthService {
public:
  // Constructor: recibe una referencia a la configuración y a la conexión de
  // base de datos
  AuthService(const Config &config, std::shared_ptr<pqxx::connection> dbConn);

  // Método para registrar un usuario. Podría lanzar excepciones en caso de
  // error.
  void registerUser(const std::string &username, const std::string &password);

  // Método para autenticar un usuario. Devuelve el token JWT en caso de éxito.
  std::string loginUser(const std::string &username,
                        const std::string &password);

private:
  Config config_;
  std::shared_ptr<pqxx::connection> dbConn_;
  Logger logger_;

  // Métodos privados auxiliares
  std::string hashPassword(const std::string &password);
  bool verifyPassword(const std::string &password, const std::string &hashed);
  std::string generateJWT(const std::string &username);
};

#endif // AUTHSERVICE_HPP

// src/auth/AuthService.hpp
#pragma once
#include <jwt-cpp/jwt.h>
#include <pqxx/pqxx> // PostgreSQL C++ client
#include <string>

struct User {
  int id;
  std::string username;
  std::string password_hash;
};

class AuthService {
public:
  AuthService(const std::string &db_connection_str);

  bool registerUser(const std::string &username, const std::string &password);
  std::string loginUser(const std::string &username,
                        const std::string &password);
  bool validateToken(const std::string &token);

private:
  std::string db_connection_str;
  std::string jwt_secret = "tu_secreto_secreto";

  User getUserByUsername(const std::string &username);
  std::string hashPassword(const std::string &password);
};

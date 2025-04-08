// src/auth/AuthService.cpp
#include "../../include/auth/AuthService.hpp"
#include <iomanip>
#include <openssl/sha.h>
#include <sstream>

AuthService::AuthService(const std::string &db_conn_str)
    : db_connection_str(db_conn_str) {}

std::string AuthService::hashPassword(const std::string &password) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, password.c_str(), password.size());
  SHA256_Final(hash, &sha256);

  std::stringstream ss;
  for (unsigned char i : hash) {
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
  }
  return ss.str();
}

bool AuthService::registerUser(const std::string &username,
                               const std::string &password) {
  try {
    pqxx::connection conn(db_connection_str);
    pqxx::work txn(conn);

    std::string hashed_pw = hashPassword(password);

    txn.exec_params(
        "INSERT INTO users (username, password_hash) VALUES ($1, $2)", username,
        hashed_pw);
    txn.commit();
    return true;
  } catch (const pqxx::unique_violation &e) {
    // Username ya existe
    return false;
  } catch (const std::exception &e) {
    // Otros errores
    return false;
  }
}

std::string AuthService::loginUser(const std::string &username,
                                   const std::string &password) {
  try {
    User user = getUserByUsername(username);
    std::string hashed_input = hashPassword(password);

    if (user.password_hash == hashed_input) {
      auto token =
          jwt::create()
              .set_issuer("MOM")
              .set_type("JWS")
              .set_payload_claim("user_id", jwt::claim(std::to_string(user.id)))
              .set_payload_claim("username", jwt::claim(user.username))
              .sign(jwt::algorithm::hs256{jwt_secret});

      return token;
    }
    return "";
  } catch (...) {
    return "";
  }
}

User AuthService::getUserByUsername(const std::string &username) {
  pqxx::connection conn(db_connection_str);
  pqxx::work txn(conn);

  auto result = txn.exec_params1(
      "SELECT id, username, password_hash FROM users WHERE username = $1",
      username);

  User user;
  user.id = result[0].as<int>();
  user.username = result[1].as<std::string>();
  user.password_hash = result[2].as<std::string>();

  return user;
}

bool AuthService::validateToken(const std::string &token) {
  try {
    auto decoded = jwt::decode(token);
    jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{jwt_secret})
        .verify(decoded);
    return true;
  } catch (...) {
    return false;
  }
}

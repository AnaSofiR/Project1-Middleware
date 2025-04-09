// src/auth/AuthService.cpp
#include "../../include/auth/AuthService.hpp"
#include <chrono>
// Puedes incluir aquí alguna librería para hashing de contraseñas, por ejemplo,
// bcrypt

AuthService::AuthService(const Config &config,
                         std::shared_ptr<pqxx::connection> dbConn)
    : config_(config), dbConn_(dbConn), logger_("AuthService") {
  // Inicialización adicional si es necesaria.
}

void AuthService::registerUser(const std::string &username,
                               const std::string &password) {
  // Ejemplo simplificado de hashing (deberías usar una librería robusta para
  // ello)
  std::string hashedPassword = hashPassword(password);

  try {
    pqxx::work txn(*dbConn_);
    // Suponiendo que la tabla de usuarios se llama "users" con columnas
    // "username" y "password"
    txn.exec_params("INSERT INTO users (username, password) VALUES ($1, $2)",
                    username, hashedPassword);
    txn.commit();
    logger_.info("Usuario registrado exitosamente: " + username);
  } catch (const std::exception &e) {
    logger_.error("Error al registrar usuario: " + std::string(e.what()));
    throw;
  }
}

std::string AuthService::loginUser(const std::string &username,
                                   const std::string &password) {
  try {
    pqxx::nontransaction ntx(*dbConn_);
    // Obtenemos el hash almacenado en la base de datos
    pqxx::result r = ntx.exec_params(
        "SELECT password FROM users WHERE username = $1", username);

    if (r.empty()) {
      throw std::runtime_error("Usuario no encontrado");
    }

    std::string storedHash = r[0][0].as<std::string>();

    if (!verifyPassword(password, storedHash)) {
      throw std::runtime_error("Contraseña incorrecta");
    }

    // Si la autenticación es exitosa, se genera el JWT
    std::string token = generateJWT(username);
    logger_.info("Usuario autenticado exitosamente: " + username);
    return token;
  } catch (const std::exception &e) {
    logger_.error("Error al autenticar usuario: " + std::string(e.what()));
    throw;
  }
}

std::string AuthService::hashPassword(const std::string &password) {

  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;

  SHA256_Init(&sha256);
  SHA256_Update(&sha256, password.c_str(), password.size());
  SHA256_Final(hash, &sha256);

  // Convert hash to hex string
  std::stringstream ss;
  for (unsigned char c : hash)
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
  return ss.str();
}

bool AuthService::verifyPassword(const std::string &password,
                                 const std::string &hashed) {
  // Implementa la verificación correspondiente, comparando el hash del password
  // ingresado con el almacenado.
  return hashPassword(password) == hashed;
}

std::string AuthService::generateJWT(const std::string &username) {
  // Usando jwt-cpp para generar el token
  auto token = jwt::create()
                   .set_issuer("mi_api")
                   .set_type("JWS")
                   .set_audience("usuarios")
                   .set_subject(username)
                   .set_issued_at(std::chrono::system_clock::now())
                   .set_expires_at(std::chrono::system_clock::now() +
                                   std::chrono::hours(24))
                   .sign(jwt::algorithm::hs256{
                       config_.getJWTSecret()}); // Se asume que Config tiene un
                                                 // método getJWTSecret()

  return token;
}

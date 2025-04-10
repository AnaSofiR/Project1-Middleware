// src/test_utils.cpp
#include "../include/utils/Config.hpp"
#include "../include/utils/Logger.hpp"
#include <iostream>

int main() {
  try {
    // Instancia y prueba del Config
    Config config("src/config.env");

    std::string jwtSecret = config.getJWTSecret();
    std::string dbConn = config.getDBConnectionString();

    std::cout << "JWT Secret: " << jwtSecret << std::endl;
    std::cout << "DB Connection String: " << dbConn << std::endl;

    // Instancia y prueba del Logger
    Logger logger("TestModule", LogLevel::DEBUG);

    logger.info("Esto es un mensaje informativo.");
    logger.warn("Este es un mensaje de advertencia.");
    logger.error("Este es un mensaje de error.");
    logger.debug("Este es un mensaje de depuración.");

  } catch (const std::exception &ex) {
    std::cerr << "Error en la prueba: " << ex.what() << std::endl;
  }

  return 0;
}

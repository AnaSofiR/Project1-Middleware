#include "../include/auth/AuthService.hpp"
#include <iostream>

int test_auth() {
  try {
    // Cargamos config desde .env
    Config config("src/config.env");

    // Obtenemos datos
    std::string jwtSecret = config.getJWTSecret();
    std::string dbConnString = config.getDBConnectionString();

    // Creamos la conexión a la base de datos
    std::shared_ptr<pqxx::connection> dbConn =
        std::make_shared<pqxx::connection>(dbConnString);

    // Creamos instancia de AuthService
    AuthService auth(config, dbConn);

    std::string username = "juan1";
    std::string password = "mi_contraseña_segura";

    // Intentamos registrar al usuario
    auth.registerUser(username, password);
    std::cout << "Usuario registrado correctamente\n";

    // Intentamos loguearlo
    std::string token = auth.loginUser(username, password);
    std::cout << "Login exitoso. Token generado:\n" << token << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}

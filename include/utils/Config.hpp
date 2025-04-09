// include/utils/Config.hpp
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Config {
public:
  // Constructor que carga la configuración desde un archivo
  Config(const std::string &filepath) { loadConfig(filepath); }

  // Método para obtener un valor a partir de una clave
  std::string get(const std::string &key) const {
    auto it = configMap_.find(key);
    if (it != configMap_.end()) {
      return it->second;
    }
    throw std::runtime_error("Clave de configuración no encontrada: " + key);
  }

  // Método específico para obtener la clave secreta del JWT
  std::string getJWTSecret() const { return get("JWT_SECRET"); }

  // Método para obtener la cadena de conexión a la base de datos
  std::string getDBConnectionString() const { return get("DB_CONNECTION"); }

private:
  std::unordered_map<std::string, std::string> configMap_;

  // Función que carga el archivo de configuración
  void loadConfig(const std::string &filepath) {
    std::ifstream file(filepath);
    if (!file) {
      throw std::runtime_error(
          "No se pudo abrir el archivo de configuración: " + filepath);
    }
    std::string line;
    while (std::getline(file, line)) {
      // Ignorar líneas vacías o comentarios (por ejemplo, líneas que inicien
      // con #)
      if (line.empty() || line[0] == '#')
        continue;

      std::istringstream iss(line);
      std::string key, value;
      if (std::getline(iss, key, '=') && std::getline(iss, value)) {
        configMap_[key] = value;
      }
    }
  }
};

#endif // CONFIG_HPP

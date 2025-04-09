#include "../../include/utils/Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

Config::Config(const std::string &filepath) { loadConfig(filepath); }

std::string Config::get(const std::string &key) const {
  auto it = configMap_.find(key);
  if (it != configMap_.end()) {
    return it->second;
  }
  throw std::runtime_error("Clave de configuración no encontrada: " + key);
}

std::string Config::getJWTSecret() const { return get("JWT_SECRET"); }

std::string Config::getDBConnectionString() const {
  return get("DB_CONNECTION");
}

void Config::loadConfig(const std::string &filepath) {
  std::ifstream file(filepath);
  if (!file) {
    throw std::runtime_error("No se pudo abrir el archivo de configuración: " +
                             filepath);
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string key, value;
    if (std::getline(iss, key, '=') && std::getline(iss, value)) {
      configMap_[key] = value;
    }
  }
}

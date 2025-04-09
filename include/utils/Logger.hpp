// include/utils/Logger.hpp
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

class Logger {
public:
  // Constructor que permite definir un nombre identificador y el nivel mínimo
  // de log
  Logger(const std::string &name, LogLevel level = LogLevel::INFO)
      : name_(name), level_(level) {}

  // Métodos para loggear en distintos niveles
  void info(const std::string &message) { log(LogLevel::INFO, message); }

  void warn(const std::string &message) { log(LogLevel::WARNING, message); }

  void error(const std::string &message) { log(LogLevel::ERROR, message); }

  void debug(const std::string &message) { log(LogLevel::DEBUG, message); }

private:
  std::string name_;
  LogLevel level_;
  std::mutex mutex_;

  // Obtiene la hora actual en formato HH:MM:SS
  std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
#if defined(_MSC_VER) || defined(__MINGW32__)
    localtime_s(&buf, &in_time);
#else
    localtime_r(&in_time, &buf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&buf, "%H:%M:%S");
    return oss.str();
  }

  // Función interna para imprimir el mensaje si el nivel es adecuado
  void log(LogLevel msgLevel, const std::string &message) {
    // En este ejemplo se imprimen todos los mensajes; podrías filtrar por nivel
    // si lo deseas
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "[" << getCurrentTime() << "][" << name_ << "]["
              << levelToString(msgLevel) << "] " << message << std::endl;
  }

  // Convierte el enum LogLevel a una cadena
  std::string levelToString(LogLevel level) {
    switch (level) {
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::DEBUG:
      return "DEBUG";
    default:
      return "UNKNOWN";
    }
  }
};

#endif // LOGGER_HPP

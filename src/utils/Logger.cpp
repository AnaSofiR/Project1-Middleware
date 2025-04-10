
#include "../../include/utils/Logger.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

Logger::Logger(const std::string &name, LogLevel level)
    : name_(name), level_(level) {}

void Logger::info(const std::string &message) { log(LogLevel::INFO, message); }
void Logger::warn(const std::string &message) {
  log(LogLevel::WARNING, message);
}
void Logger::error(const std::string &message) {
  log(LogLevel::ERROR, message);
}
void Logger::debug(const std::string &message) {
  log(LogLevel::DEBUG, message);
}

std::string Logger::getCurrentTime() {
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

void Logger::log(LogLevel msgLevel, const std::string &message) {
  std::lock_guard<std::mutex> lock(mutex_);
  std::cout << "[" << getCurrentTime() << "][" << name_ << "]["
            << levelToString(msgLevel) << "] " << message << std::endl;
}

std::string Logger::levelToString(LogLevel level) {
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

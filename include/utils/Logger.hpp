
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <mutex>
#include <string>

enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

class Logger {
public:
  Logger(const std::string &name, LogLevel level = LogLevel::INFO);

  void info(const std::string &message);
  void warn(const std::string &message);
  void error(const std::string &message);
  void debug(const std::string &message);

private:
  std::string name_;
  LogLevel level_;
  std::mutex mutex_;

  std::string getCurrentTime();
  void log(LogLevel msgLevel, const std::string &message);
  std::string levelToString(LogLevel level);
};

#endif // LOGGER_HPP

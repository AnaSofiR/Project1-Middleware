
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>

class Config {
public:
  Config(const std::string &filepath);

  std::string get(const std::string &key) const;
  std::string getJWTSecret() const;
  std::string getDBConnectionString() const;

private:
  std::unordered_map<std::string, std::string> configMap_;
  void loadConfig(const std::string &filepath);
};

#endif // CONFIG_HPP

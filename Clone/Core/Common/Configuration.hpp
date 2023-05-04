#pragma once
#include <exception>
#include <map>
#include <string>

namespace Clone::Parser {

enum class CopyingMode {
  BitStream,           // byte per byte copying in multithread way
  SharedMemoryStream,  // per data chunks shared via unified memory
};

struct CfgProperties {
  static inline const char *source = "source";
  static inline const char *destination = "destination";
  static inline const char *frame = "frame";
  static inline const char *mode = "ipc";
  static inline const char *role = "role";
};

class Configuration {
 public:
  explicit Configuration() = default;
  Configuration(const Configuration &) = default;
  ~Configuration() = default;

  void set_param(std::string key, std::string value)
  {
    if (_properties.find(key) == _properties.end())
      _properties.emplace(std::move(key), std::move(value));
    else
      throw std::exception("Parameter duplication");
  }

  template <typename return_t>
  return_t get_param(const std::string & key)
  {
    throw std::bad_cast();
  }

  template <>
  std::string get_param<std::string>(const std::string & key)
  {
    if (auto search = _properties.find(key); search != _properties.end())
      return search->second;
    throw std::exception();
  }

  template <>
  CopyingMode get_param<CopyingMode>(const std::string & key)
  {
    // todo need more checks
    if (auto search = _properties.find(key); search != _properties.end())
    {
      if (search->second.compare("IPC") == 0) return CopyingMode::SharedMemoryStream;
      if (search->second.compare("BITESTREAM") == 0) return CopyingMode::BitStream;
    }
    throw std::exception("Bad key: ");
  }

  void print()
  {
    for (const auto &p : _properties) {
      std::cout << p.first << " => " << p.second << '\n';
    }
  }

 private:
  std::map<std::string, std::string> _properties;
};
}  // namespace Clone::Parser
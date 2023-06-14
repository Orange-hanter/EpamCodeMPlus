#pragma once

#include <exception>
#include <map>
#include <stdexcept>
#include <string>

namespace Clone::Parser
{

enum class CopyingMode : int
{
    BitStream,           // byte per byte copying in multithread way
    SharedMemoryStream,  // per data chunks shared via unified memory
    NetworkSharing,
};

struct CfgProperties
{
    static inline const char* source      = "source";
    static inline const char* destination = "destination";
    static inline const char* frame       = "frame";
    static inline const char* mode        = "ipc";
    static inline const char* role        = "role";
};

inline namespace Internal
{


template <typename ReturnType>
struct ConfigurationHelper
{
    static ReturnType get_param(const std::string& param_name, const std::map<std::string, std::string>* _properties);
};

template <>
struct ConfigurationHelper<std::string>
{
    static std::string get_param(const std::string& param_name, const std::map<std::string, std::string>* _properties)
    {
        if (auto search = _properties->find(param_name); search != _properties->end())
            return search->second;
        throw std::runtime_error("Bad key: ");
    }
};

template <>
struct ConfigurationHelper<CopyingMode>
{
    static CopyingMode get_param(const std::string& param_name, const std::map<std::string, std::string>* _properties)
    {
        if (auto search = _properties->find(param_name); search != _properties->end())
        {
            if (search->second == "IPC") return CopyingMode::SharedMemoryStream;
            if (search->second == "BITESTREAM") return CopyingMode::BitStream;
            if (search->second == "NET") return CopyingMode::NetworkSharing;
        }
        throw std::runtime_error("Bad key: ");
    }
};
}  // namespace Internal

class Configuration
{
public:
    explicit Configuration()            = default;
    Configuration(const Configuration&) = default;
    ~Configuration()                    = default;

    void set_param(std::string key, std::string value)
    {
        if (_properties.find(key) == _properties.end())
            _properties.emplace(std::move(key), std::move(value));
        else
            throw std::invalid_argument("Parameter duplication");
    }

    template <typename return_t = std::string>
    return_t get_param(const std::string& key)
    {
        return ConfigurationHelper<return_t>::get_param(key, &_properties);
    }

private:
    std::map<std::string, std::string> _properties;
};

}  // namespace Clone::Parser
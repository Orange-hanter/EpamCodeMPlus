#pragma once
#include <exception>
#include <string>
#include <map>

/*
we have next fields
- 
- 
- 
- 
- 
*/
namespace Clone::Parser {
    
    struct CfgProperties {
        static inline const char * source = "source";
        static inline const char * destination = "destination";
        static inline const char * frame = "frame";
        static inline const char * mode = "mode";
        static inline const char * role = "role";
    };

    class Configuration
    {
        
    public:
        explicit Configuration() = default;
        Configuration(const Configuration & ) = default;
        ~Configuration() = default;

        void set_param(std::string key, std::string value)
        {
            _properties.emplace(key, value);
        }

        template<typename return_t>
        return_t get_param(std::string key){
                if(auto search = _properties.find(key); search != _properties.end())
                    return static_cast<return_t>(search->second);
                throw std::exception();
        }

        std::string get_param(std::string key)
        {
            if(auto search = _properties.find(key); search != _properties.end())
                    return search->second;
                throw std::exception();
        }

        private:
        std::map<std::string, std::string> _properties;
    };
}
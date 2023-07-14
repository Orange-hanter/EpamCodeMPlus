//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_NETWORKWORKERFACTORY_HPP
#define EPAMMIDDLE_NETWORKWORKERFACTORY_HPP

#include "AbstractWorkerFactory.hpp"

namespace Clone {

class NetworkWorkerFactory : public AbstractWorkerFactory {
 public:
  explicit NetworkWorkerFactory(std::string source, std::string ip, std::string port)
      : _source(std::move(source)),
        _adr(std::move(ip)),
        _port(std::move(port))
  {
  }

  NetworkWorkerFactory() = delete;


  Clone::Workers::IWriter* CreateWriter() override;

  Clone::Workers::IReader* CreateReader() override;

 private:
  std::string _source;
  std::string _adr;
  std::string _port;
};


}  // namespace Clone

#endif  // EPAMMIDDLE_NETWORKWORKERFACTORY_HPP

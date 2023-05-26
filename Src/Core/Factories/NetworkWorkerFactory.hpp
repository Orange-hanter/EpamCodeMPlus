//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_NETWORKWORKERFACTORY_HPP
#define EPAMMIDDLE_NETWORKWORKERFACTORY_HPP

#include "AbstractWorkerFactory.hpp"

namespace Clone {

class NetworkWorkerFactory : public AbstractWorkerFactory {
 public:
  explicit NetworkWorkerFactory(std::string source)
      : _source(std::move(source))
  {
  }

  NetworkWorkerFactory() = delete;


  Clone::Workers::IWriter* CreateWriter() override;

 private:
  Clone::Workers::IReader* CreateReader() override;
  std::string _source;
  std::string _adres;
  std::string _port;
};


}  // namespace Clone

#endif  // EPAMMIDDLE_NETWORKWORKERFACTORY_HPP

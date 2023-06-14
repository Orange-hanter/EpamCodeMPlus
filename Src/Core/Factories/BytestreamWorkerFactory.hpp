//
// Created by daniil on 6/7/23.
//

#ifndef EPAMMIDDLE_BYTESTREAMWORKERFACTORY_HPP
#define EPAMMIDDLE_BYTESTREAMWORKERFACTORY_HPP

#include <string>

#include "AbstractWorkerFactory.hpp"

namespace Clone{

class BytestreamWorkerFactory : public AbstractWorkerFactory {
 public:
  explicit BytestreamWorkerFactory(std::string source, std::string destination)
      : _source(std::move(source)), _destination(std::move(destination))
  {
  }

  BytestreamWorkerFactory() = delete;


  Clone::Workers::IWriter* CreateWriter() override;

 private:
  Clone::Workers::IReader* CreateReader() override;
  std::string _source;
  std::string _destination;
};

}

#endif  // EPAMMIDDLE_BYTESTREAMWORKERFACTORY_HPP

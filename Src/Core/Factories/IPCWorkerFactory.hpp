//
// Created by daniil on 6/7/23.
//

#ifndef EPAMMIDDLE_IPCWORKERFACTORY_HPP
#define EPAMMIDDLE_IPCWORKERFACTORY_HPP

#include "AbstractWorkerFactory.hpp"

namespace Clone
{

class AbstractIPCFactory : public AbstractWorkerFactory {
 public:
  explicit AbstractIPCFactory(std::string source) : _source(std::move(source)) {}

  AbstractIPCFactory() = delete;

  Clone::Workers::IReader* CreateReader() override;

  Clone::Workers::IWriter* CreateWriter() override;

 private:
  std::string _source;
};

} // Clone

#endif  // EPAMMIDDLE_IPCWORKERFACTORY_HPP

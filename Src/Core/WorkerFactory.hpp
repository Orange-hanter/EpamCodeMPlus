#pragma once

#include <memory>
#include <string>
#include <utility>

#include "AbstractWorkerFactory.hpp"
#include "Workers.h"

namespace Clone {

namespace Parser {
    class Configuration;
    enum class CopyingMode;
}

class AbstractIPCFactory;
class BytestreamWorkerFactory;
class AbstractWorkerFactory;

using spIWorker = std::shared_ptr<Workers::IWorker>;

/**
 *    @brief Class that produce work
 */
class WorkerFactory {
 public:


  explicit WorkerFactory(std::shared_ptr<Parser::Configuration> sp_cfg);
  ~WorkerFactory() = default;

  Clone::Workers::IWorker* getWorker();

 private:
  //AbstractWorkerFactory* getFactory(std::string_view mode);
  AbstractWorkerFactory* getFactory(Parser::CopyingMode mode);

  std::shared_ptr<Parser::Configuration> _config;
};

class AbstractIPCFactory : public AbstractWorkerFactory {
 public:
  explicit AbstractIPCFactory(std::string source) : _source(std::move(source)) {}

  AbstractIPCFactory() = delete;

  Clone::Workers::IReader* CreateReader() override;

  Clone::Workers::IWriter* CreateWriter() override;

 private:
  std::string _source;
};

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


}  // namespace Clone
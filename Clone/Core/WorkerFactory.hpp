#pragma once

#include <memory>
#include <string>

#include "AbstractWorkerFactory.hpp"
#include "Workers.h"

namespace Clone {

namespace Parser {
    class Configuration;
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
  enum class CopyingMode {
    BitStream,           // byte per byte copying in multithread way
    SharedMemoryStream,  // per data chunks shared via unified memory
  };

  WorkerFactory(std::shared_ptr<Parser::Configuration> sp_cfg);
  ~WorkerFactory() = default;

  Clone::Workers::IWorker* getWorker();

 private:
  AbstractWorkerFactory* getFactory(std::string mode);
  AbstractWorkerFactory* getFactory(CopyingMode mode);

  std::shared_ptr<Parser::Configuration> _config;
};

class AbstractIPCFactory : public AbstractWorkerFactory {
 public:
  explicit AbstractIPCFactory(std::string source) : _source(source) {}

  AbstractIPCFactory() = delete;

  Clone::Workers::IReader* CreateReader() override;

  Clone::Workers::IWriter* CreateWriter() override;

 private:
  std::string _source;
};

class BytestreamWorkerFactory : public AbstractWorkerFactory {
 public:
  explicit BytestreamWorkerFactory(std::string source, std::string destination)
      : _source(source), _destination(destination)
  {
  }

  BytestreamWorkerFactory() = delete;

  Clone::Workers::IReader* CreateReader() override;

  Clone::Workers::IWriter* CreateWriter() override;

 private:
  std::string _source;
  std::string _destination;
};

}  // namespace Clone
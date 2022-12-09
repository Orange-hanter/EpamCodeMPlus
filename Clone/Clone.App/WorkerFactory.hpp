#pragma once

#include <memory.h>
#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "Workers/Workers.h"

namespace Clone {

/*
@brief Class that produce work
*/
class WorkerFactory {
  using spIWorker = std::shared_ptr<Workers::IWorker>;

 public:
  enum class CopyingMode {
    BitStream,           // byte per byte copying in multithread way
    SharedMemoryStream,  // TODO: need to implement
  };

 public:
  WorkerFactory();
  WorkerFactory(std::string source, std::string target);
  ~WorkerFactory();

  Workers::IWorker* getWorker(std::string source, std::string target,
                              CopyingMode mode = CopyingMode::BitStream);

  Workers::IWorker* getWorker(std::filesystem::path src,
                              std::filesystem::path dst,
                              CopyingMode mode = CopyingMode::BitStream);

  Workers::IWorker* getWorker(CopyingMode mode = CopyingMode::BitStream);

 private:
  CopyingMode _mode;
  std::string _source;
  std::string _target;
  //std::map<CopyingMode, spIWorker> _registeredWorkers; // delete
};
}  // namespace Clone
#pragma once
#include <memory.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "Utils.hpp"
#include "iWorker.hpp"

namespace Clone {

/*
@brief Class that produce work
*/
class Director {
  using spIWorker = std::shared_ptr<Workers::IWorker>;

 public:
  Director(std::string source, std::string target,
           CopyingMode mode = CopyingMode::BitStream);

  Director(std::filesystem::path source, std::filesystem::path target,
           CopyingMode mode = CopyingMode::BitStream);
  ~Director();

  void work();

  bool processStatus();

 private:
  CopyingMode _mode;
  std::map<CopyingMode, spIWorker> _registeredWorkers;
};
}  // namespace Clone
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
  using spIworker = std::shared_ptr<Workers::IWorker>;

 public:
  Director(std::string source, std::string target,
           CopyingMode mode = CopyingMode::Bytestream);

  Director(std::filesystem::path source, std::filesystem::path target,
           CopyingMode mode = CopyingMode::Bytestream);
  ~Director();

  void work();

 private:
  CopyingMode _mode;
  std::map<CopyingMode, spIworker> _registeredWorkers;
};
}  // namespace Clone
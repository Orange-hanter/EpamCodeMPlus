#pragma once

#include <memory>
#include <string>

#include "Workers/Workers.h"


namespace Clone {


/*
@brief Class that produce work
*/
class WorkerFactory {
 public:
  using spIWorker = std::shared_ptr<Workers::IWorker>;
  enum class CopyingMode {
    BitStream,           // byte per byte copying in multithread way
    SharedMemoryStream,  // per data chunks shared via unified memory 
  };

  explicit WorkerFactory(std::string_view source, std::string_view target);

  ~WorkerFactory() = default;

  spIWorker getWorker(std::string source, std::string target,
                      CopyingMode mode = CopyingMode::BitStream) const;

  spIWorker getWorker(const std::filesystem::path& src,
                      const std::filesystem::path& dst,
                      CopyingMode mode = CopyingMode::BitStream) const;

  spIWorker getWorker(CopyingMode mode = CopyingMode::BitStream) const;

  spIWorker getWorker(CopyingMode mode, bool isWriter);

 private:
  CopyingMode _mode;
  Workers::ROLE _role;
  std::string _source;
  std::string _target;
};
}  // namespace Clone
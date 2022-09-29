#include "Director.hpp"

#include <memory>
#include <typeinfo>

namespace Clone {

namespace fs = std::filesystem;

inline fs::path ManipulateWithName(fs::path file_path, fs::path folder_path)
{
  // todo add handling cases when new file should be at the same folder and name
  // is not specified
  if (fs::is_regular_file(fs::status(folder_path))) {
    return folder_path;
  }
  return folder_path.append(fs::path(file_path).filename().string());
}

std::string ManipulateWithName(std::string file_path, std::string folder_path)
{
  return ManipulateWithName(fs::path(file_path), fs::path(folder_path))
      .string();
}

Director::Director(std::string src, std::string dst, CopyingMode mode)
    : _mode(mode)
{
  // assert(src != dst);
  using Workers::ByteStreamWorker;
  dst = ManipulateWithName(src, dst);
  _registeredWorkers.try_emplace(CopyingMode::BitStream,
                                 std::make_shared<ByteStreamWorker>(src, dst));
}

Director::Director(fs::path src, fs::path dst, CopyingMode mode) : _mode(mode)
{
  // assert(src != dst);
  using Workers::ByteStreamWorker;
  dst = ManipulateWithName(src, dst);
  _registeredWorkers.try_emplace(
      CopyingMode::BitStream,
      std::make_shared<ByteStreamWorker>(src.string(), dst.string()));
}

Director::~Director() {}

void Director::work() { _registeredWorkers[_mode]->transferring(); }

bool Director::processStatus() { return _registeredWorkers[_mode]->isDone(); }
}  // namespace Clone

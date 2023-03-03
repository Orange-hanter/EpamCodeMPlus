#include "WorkerFactory.hpp"
#include "Utils.hpp"

#include <memory>
#include <typeinfo>


namespace Clone {

namespace fs = std::filesystem;

using Workers::ByteStreamWorker;
using Workers::IPCWorker;
using Workers::IWorker;

WorkerFactory::WorkerFactory(std::string_view source, std::string_view target)
    : _source(source), _target(target)
{
}

WorkerFactory::spIWorker WorkerFactory::getWorker(std::string source,
                                                  std::string target,
                                                  CopyingMode mode) const
{
  target = Utils::ManipulateWithName(source, target);

  spIWorker workerObj;
  switch (mode) {
    case CopyingMode::BitStream:
      workerObj = std::make_shared<ByteStreamWorker>(source, target);
      break;

    case CopyingMode::SharedMemoryStream:

      workerObj = std::make_shared<IPCWorker>(source, _role);
      break;

    default:
      // Do nothing
      break;
  }
  return workerObj;
}

WorkerFactory::spIWorker WorkerFactory::getWorker(const fs::path& src,
                                                  const fs::path& dst,
                                                  CopyingMode mode) const
{
  return getWorker(src.string(), dst.string(), mode);
}

WorkerFactory::spIWorker WorkerFactory::getWorker(CopyingMode mode) const
{
  return getWorker(_source, _target, mode);
}

WorkerFactory::spIWorker WorkerFactory::getWorker(CopyingMode mode, bool isWriter)
{
  _role = isWriter ? Workers::ROLE::WRITER : Workers::ROLE::READER;
  return getWorker(_source, _target, mode);
}

}  // namespace Clone

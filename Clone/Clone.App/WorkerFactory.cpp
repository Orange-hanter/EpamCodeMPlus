#include "WorkerFactory.hpp"

#include <memory>
#include <typeinfo>

#include "Utils.hpp"

namespace Clone {

using Workers::ByteStreamWorker;
using Workers::IWorker;

WorkerFactory::WorkerFactory()
{
  // assert(src != dst);
}

WorkerFactory::WorkerFactory(std::string source, std::string target): _target(target), _source(source) {}

WorkerFactory::~WorkerFactory() {}

IWorker* WorkerFactory::getWorker(std::string source, std::string target,
                                  CopyingMode mode)
{
  target = ManipulateWithName(source, target);

  spIWorker result;
  switch (mode) {
    case CopyingMode::BitStream:
      result = std::make_shared<ByteStreamWorker>(source, target);
      break;

    case CopyingMode::SharedMemoryStream:
      break;

    default:
      // Do nothing
      break;
  }
  return result.get();
}

IWorker* WorkerFactory::getWorker(fs::path src, fs::path dst, CopyingMode mode)
{
  return getWorker(src.string(), dst.string(), mode);
}

Workers::IWorker* WorkerFactory::getWorker(CopyingMode mode) {
  return getWorker(_source, _target, mode);
}

}  // namespace Clone

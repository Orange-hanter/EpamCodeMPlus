#pragma once
#include "Director.hpp"

#include <memory>
#include <typeinfo>

namespace Clone {

Director::Director(std::string src, std::string dst, CopyingMode mode)
    : _mode(mode)
{
  using Workers::ByteStreamWorker;
  _registeredWorkers.try_emplace(CopyingMode::Bytestream,
                                 std::make_shared<ByteStreamWorker>(src, dst));
}

Director::~Director() {}

void Director::work()
{
  _registeredWorkers[_mode]->transferring();
  while (!_registeredWorkers[_mode]->isDone()) {
    ;
  }
}
}  // namespace Clone

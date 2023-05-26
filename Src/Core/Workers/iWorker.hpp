#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "Frame.hpp"
#include "SafeDeque.hpp"
#include "iTransportLayer.hpp"

namespace Clone::Workers {

using std::ios;
using TransportLayer::iTransportLayer;

class IWorker {
 public:
  virtual int execute() = 0;
  virtual bool isDone() = 0;
  virtual ~IWorker() = default;
};

class IReader : public IWorker {
 public:
  ~IReader() override = default;
  IReader() = default;

 protected:
  iTransportLayer* _transport{nullptr};
};

class IWriter : public IWorker {
 public:
  ~IWriter() override = default;
  IWriter() = default;
  IWriter(iTransportLayer* tl) : _transport(tl) {}

 protected:
  iTransportLayer* _transport{nullptr};
};

}  // namespace Clone::Workers
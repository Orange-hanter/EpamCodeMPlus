#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "Frame.hpp"
#include "SafeDeque.hpp"

namespace Clone::Workers {

using std::ios;

class IWorker {
 public:
  virtual int execute() = 0;
  virtual bool isDone() = 0;
  virtual ~IWorker() = default;
};

using IReader = IWorker;
using IWriter = IWorker;


}  // namespace Clone::Workers
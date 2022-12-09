#pragma once
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

#include "Frame.hpp"
#include "SafeDeque.hpp"

namespace Clone {
namespace Workers {

using namespace std::literals::chrono_literals;
using std::ios;

class IWorker {
 public:
  virtual int execute() = 0;
  virtual bool isDone() = 0;
  virtual ~IWorker(){};

 protected:
  std::mutex m;
  std::condition_variable cv;
  std::thread writer;
  ThreadSafeDeque<Frame<>> _packages;
};

}  // namespace Workers
}  // namespace Clone
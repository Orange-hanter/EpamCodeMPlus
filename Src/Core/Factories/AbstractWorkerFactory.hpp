#pragma once

#include "iWorker.hpp"

namespace Clone {

class AbstractWorkerFactory {
 public:
  virtual Clone::Workers::IReader* CreateReader() = 0;
  virtual Clone::Workers::IWriter* CreateWriter() = 0;
};

}  // namespace Clone

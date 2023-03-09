#pragma once
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

class IReader: public IWorker
{
  public:
    ~IReader() override = default;
};


class IWriter: public IWorker
{
  public:
    ~IWriter() override = default;
};

}  // namespace Clone::Workers
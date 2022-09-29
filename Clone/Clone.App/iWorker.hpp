#pragma
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
  virtual int transferring() = 0;
  virtual bool isDone() = 0;
  virtual ~IWorker(){};

 protected:
  std::mutex m;
  std::condition_variable cv;
  std::thread writer;
  ThreadSafeDeque<Frame<>> _packages;
};

class ByteStreamWorker : public IWorker {
 public:
  ByteStreamWorker() = delete;

  explicit ByteStreamWorker(std::string src, std::string dst)
      : _src(std::make_unique<std::ifstream>(src, ios::binary)),
        _dst(std::make_unique<std::ofstream>(dst, ios::binary))
  {
  }

  int transferring() override
  {
    startProcedureOfWriting();
    readDataToBuf();
    return 0;
  }

  bool isDone() override { return _done; }

  inline void startProcedureOfWriting()
  {
    auto copyingAlgorithm = [this]() {
      while (!_src->eof() || !_packages.empty()) {
        Frame pkg;
        _packages.pop_front_waiting(pkg);
        (*_dst).write(pkg.data(), pkg.frameSize());
      }

      _src->close();
      _dst->close();
      _done = true;
    };
    writer = std::thread(copyingAlgorithm);
  }

  inline void readDataToBuf()
  {
    for (; !_src->eof();) {
      Frame frame;
      (*_src) >> frame;
      _packages.push_back(std::move(frame));
      cv.notify_one();
    }
    cv.notify_one();
  }

  ~ByteStreamWorker() { writer.join(); }

 private:
  std::unique_ptr<std::ofstream> _dst;
  std::unique_ptr<std::ifstream> _src;
  bool _done = false;
};
}  // namespace Workers
}  // namespace Clone
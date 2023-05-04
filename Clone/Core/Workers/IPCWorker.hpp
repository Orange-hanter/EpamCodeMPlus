#include <array>
#include <atomic>
#include <cassert>

#include "iWorker.hpp"

namespace Clone::Workers {

using Clone::IPC::Frame;

enum class ROLE {
  READER,
  WRITER
};

class IPCWriter final : public IWriter {
 public:
  IPCWriter() = delete;

  explicit IPCWriter(std::string src)
  {
    _src = std::make_unique<std::ifstream>(src, ios::binary);
    if (_src->is_open())
      std::cout << "Opened file " << src << " to write\n";
    else
      assert("BAD INITIALISATION");
  }

  int execute() override
  {
    do {
      ;
    } while (_frame.isState(Frame::STATE::INITIALISED));

    _frame.setState(Frame::STATE::READY_TO_WRITE);

    while (!_src->eof()) {
      if (_frame.isState(Frame::STATE::READY_TO_WRITE)) {
        (*_src) >> _frame;
        _frame.logDataBulk();
        _frame.setState(Frame::STATE::READY_TO_READ);
      }
    }
    do {
      ;
    } while (not _frame.isState(Frame::STATE::READY_TO_WRITE));

    _frame.setState(Frame::STATE::END_SOURCE);
    return 0;
  }

  bool isDone() override { return _frame.isState(Frame::STATE::DONE); }

 private:
  std::unique_ptr<std::ifstream> _src;
  Frame _frame;
};

class IPCReader final : public IReader {
 public:
  IPCReader() = delete;

  explicit IPCReader(std::string src)
  {
    _dst = std::make_unique<std::ofstream>(src, ios::binary);
    if (_dst->is_open())
      std::cout << "Opened file " << src << " to write\n";
    else
      assert("BAD INITIALISATION");
  }

  ~IPCReader() override = default;

  int execute() override
  {
    do {
      ;
    } while (_frame.isState(Frame::STATE::INITIALISED));

    while (not _frame.isState(Frame::STATE::END_SOURCE)) {
      if (_frame.isState(Frame::STATE::READY_TO_READ)) {
        _frame.logDataBulk();
        (*_dst) << _frame;
        _frame.setState(Frame::STATE::READY_TO_WRITE);
      }
    }
    _frame.setState(Frame::STATE::DONE);
    return 0;
  }

  bool isDone() override { return _frame.isState(Frame::STATE::DONE); }

 private:
  std::unique_ptr<std::ofstream> _dst;
  Frame _frame;
};

}  // namespace Clone::Workers
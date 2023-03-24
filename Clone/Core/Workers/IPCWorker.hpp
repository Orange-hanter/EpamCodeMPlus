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

class IPCWorker : public IWorker {
 public:
  IPCWorker() = delete;

  explicit IPCWorker(std::string src, ROLE role) : _role(role)
  {
    if (role == ROLE::WRITER) {
      _src = std::make_unique<std::ifstream>(src, ios::binary);
      _dst = nullptr;
      std::cout << "Opened file " << src << " to read\n";
    }
    else {
      _src = nullptr;
      _dst = std::make_unique<std::ofstream>(src, ios::binary);
      if (_dst->is_open())
        std::cout << "Opened file " << src << " to write\n";
      else
        assert("BAD INITIALISATION");
    }
  }

  int execute() override
  {
    do {
      ;
    } while (frame.isState(Frame::STATE::INITIALISED));

    switch (_role) {
      case ROLE::WRITER:
        frame.setState(Frame::STATE::READY_TO_WRITE);
        writeToSMemory();
        break;

      case ROLE::READER:
        writeFromSMemory();
        break;

      default:
        assert("WRONG_ROLE");
    }
    return 0;
  }

  bool isDone() override { return frame.isState(Frame::STATE::DONE); }

 private:
  inline void writeToSMemory()
  {
    while (!_src->eof()) {
      if (frame.isState(Frame::STATE::READY_TO_WRITE)) {
        (*_src) >> frame;
        frame.logDataBulk();
        frame.setState(Frame::STATE::READY_TO_READ);
      }
    }
    do {
      ;
    } while (not frame.isState(Frame::STATE::READY_TO_WRITE));

    frame.setState(Frame::STATE::END_SOURCE);
  }

  inline void writeFromSMemory()
  {
    while (not frame.isState(Frame::STATE::END_SOURCE)) {
      if (frame.isState(Frame::STATE::READY_TO_READ)) {
        frame.logDataBulk();
        (*_dst) << frame;
        frame.setState(Frame::STATE::READY_TO_WRITE);
      }
    }
    frame.setState(Frame::STATE::DONE);
  }

  ROLE _role;
  std::unique_ptr<std::ofstream> _dst;
  std::unique_ptr<std::ifstream> _src;
  Frame frame;
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
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

}  // namespace Clone::Workers
#include <array>

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
    }
    else {
      _src = nullptr;
      _dst = std::make_unique<std::ofstream>(src, ios::binary);
    }
  }

  int execute() override
  {
    do {
      ;
    } while (not frame.isState(Frame::STATE::READY_TO_WORK));
   
    if(_role == ROLE::WRITER) {
      frame.setState(Frame::STATE::READY_TO_WRITE);
      writeToSMemory();
    }
    else {
      writeFromSMemory();
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
        frame.setState(Frame::STATE::READY_TO_READ);
      }
    }
    frame.setState(Frame::STATE::END_SOURCE);
  }

  inline void writeFromSMemory()
  {
    while (not frame.isState(Frame::STATE::END_SOURCE)) {
      if (frame.isState(Frame::STATE::READY_TO_READ)) {
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
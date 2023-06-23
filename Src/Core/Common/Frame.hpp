#pragma once

#include "IPCFrame.hpp"

namespace Clone {

template <typename T = char>
class Frame {
 public:
  explicit Frame(size_t frameSize = defaultFrameSize) : _frameSize(frameSize)
  {
    _buf = new T[frameSize]{'\0'};
  }

  Frame(Frame&& other) noexcept
  {
    if (this == &other) return;
    _buf = other._buf;
    other._buf = nullptr;
    _frameSize = other._frameSize;
  }

  Frame(const Frame& other) : _frameSize(other._frameSize)
  {
    _buf = new T[other._frameSize];
    memcpy(_buf, other._buf, other._frameSize);
  }

  ~Frame()
  {
    delete[] _buf;
  }

  Frame& operator=(Frame&& other) noexcept
  {
    // should I check here nullptr?
    if (this != &other) {
      _buf = other._buf;
      other._buf = nullptr;
      _frameSize = other._frameSize;
    }
    return *this;
  }

  Frame& operator=(const Frame& other)
  {
    _frameSize = other._frameSize;
    _buf = new T[_frameSize];
    memcpy(_buf, other._buf, _frameSize);
    return *this;
  }

  friend std::istream& operator>>(std::istream& ifs, Frame& frame)
  {
    ifs.read(frame._buf, frame._frameSize);
    frame._frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  friend std::istream& operator>>(std::istream& ifs, Frame* frame)
  {
    assert(frame != nullptr);

    ifs.read(frame->_buf, frame->_frameSize);
    frame->_frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  T* data() const { return _buf; }

  [[nodiscard]] std::size_t frameSize() const { return _frameSize; };

 private:
  std::size_t _frameSize;
  T* _buf;

  static const size_t defaultFrameSize = 64;
};

}  // namespace Clone
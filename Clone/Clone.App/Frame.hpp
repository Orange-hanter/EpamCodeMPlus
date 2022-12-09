#pragma once

#include <fstream>

namespace Clone {

template <typename T = char>
struct Frame {

  Frame(size_t frameSize = defaultFrameSize) : _frameSize(frameSize)
  {
    _buf = new T[frameSize]{'\0'};
  }

  Frame(Frame&& other)
  {
    if (this == &other) return;
    _buf = other._buf;
    other._buf = nullptr;
    _frameSize = other._frameSize;
  }

  Frame(const Frame& other)
  {
    _frameSize = other._frameSize;
    _buf = new T[_frameSize];
    memcpy(_buf, other._buf, _frameSize);
  }

  ~Frame()
  {
    if (_buf != nullptr) delete[] _buf;
  }

  Frame& operator=(Frame&& other)
  {
    // should I check here nuppltr?
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

  friend std::ifstream& operator>>(std::ifstream& ifs, Frame& frame)
  {
    ifs.read(frame._buf, frame._frameSize);
    frame._frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  T* data() const { return _buf; }

  size_t frameSize() const { return _frameSize; };

 private:
  size_t _frameSize;
  T* _buf;

  static const size_t defaultFrameSize = 32;
};
}  // namespace Clone
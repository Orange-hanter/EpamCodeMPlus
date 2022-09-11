#pragma once

#include <fstream>

namespace Clone {

struct Frame {
  Frame(size_t frameSize = 32) : _frameSize(frameSize)
  {
    _buf = new char[frameSize]{'\0'};
  }

  Frame(Frame&& other)
  {
    if (this != &other) _buf = other._buf;
    other._buf = nullptr;
    _frameSize = other._frameSize;
  }

  ~Frame()
  {
    if (_buf != nullptr) delete[] _buf;
  }

  char* data() const { return _buf; }

  size_t frameSize() const { return _frameSize; };

  Frame& operator=(Frame&& other)
  {
    _buf = other._buf;
    other._buf = nullptr;
    _frameSize = other._frameSize;
    return *this;
  }

  friend std::ifstream& operator>>(std::ifstream& ifs, Frame& frame)
  {
    ifs.read(frame._buf, frame._frameSize);
    frame._frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

 private:
  size_t _frameSize;
  char* _buf;
};
}  // namespace Clone
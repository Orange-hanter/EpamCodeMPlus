#pragma once

#include <windows.h>

#include <cstring>
#include <istream>
#include <memory>
#include <sstream>

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

  friend std::istream& operator>>(std::istream& ifs, Frame& frame)
  {
    ifs.read(frame._buf, frame._frameSize);
    frame._frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  friend std::istream& operator>>(std::istream& ifs, Frame* frame)
  {
    ifs.read(frame->_buf, frame->_frameSize);
    frame->_frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  T* data() const { return _buf; }

  size_t frameSize() const { return _frameSize; };

 private:
  size_t _frameSize;
  T* _buf;

  static const size_t defaultFrameSize = 32;
};

struct InplaceFrame {
  using T = char;
  using TRef = T*;

  InplaceFrame(size_t frameSize = defaultFrameSize) : _frameSize(frameSize)
  {
    hMapFile =
        CreateFileMapping(INVALID_HANDLE_VALUE,  // use paging file
                          NULL,                  // default security
                          PAGE_READWRITE,        // read/write access
                          0,           // maximum object size (high-order DWORD)
                          _frameSize,  // maximum object size (low-order DWORD)
                          TEXT(szName));  // name of mapping object
    if (hMapFile == NULL) {
      ;  // do some mechanism with errors
    }
    _buf = (TRef)MapViewOfFile(hMapFile,             // handle to map object
                               FILE_MAP_ALL_ACCESS,  // read/write permission
                               0, 0, _frameSize);

    if (_buf == NULL) {
      auto err =
          GetLastError();  // the same, need to save error and check after
      CloseHandle(hMapFile);
    }
  }

  ~InplaceFrame()
  {
    UnmapViewOfFile(_buf);
    CloseHandle(hMapFile);
  }

  T* data() { return _buf; }

  friend std::basic_istream<T>& operator>>(std::basic_istream<T>& ifs,
                                           InplaceFrame& frame)
  {
    ifs.read(frame._buf, frame._frameSize);
    frame._frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  friend std::basic_istream<T>& operator>>(std::basic_istream<T>& ifs,
                                           InplaceFrame* frame)
  {
    ifs.read(frame->_buf, frame->_frameSize);
    frame->_frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  friend std::basic_ostream<char>& operator<<(std::basic_ostream<T>& ofs,
                                              InplaceFrame* frame)
  {
    ofs.write(frame->_buf, frame->_frameSize);
    return ofs;
  }

 private:
  static inline const char szName[] = {"Global\\MyFileMappingObject"};
  static const size_t defaultFrameSize = 32;
  HANDLE hMapFile;
  TRef _buf;
  size_t _frameSize;

struct DataBulk {
    T data_bulk[InplaceFrame::_frameSize];
    size_t size_of_data;
  };
};
}  // namespace Clone
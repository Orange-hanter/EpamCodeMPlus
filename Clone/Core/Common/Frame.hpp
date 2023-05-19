#pragma once

#include <windows.h>

#include <array>
#include <cstring>
#include <iostream>
#include <istream>
#include <memory>
#include <mutex>
#include <sstream>

namespace Clone {

using byte = char;

template <typename T = char>
class Frame {
 public:
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

  Frame(const Frame& other) : _frameSize(other._frameSize)
  {
    _buf = new T[other._frameSize];
    memcpy(_buf, other._buf, other._frameSize);
  }

  ~Frame()
  {
    if (_buf != nullptr) delete[] _buf;
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
    ifs.read(frame->_buf, frame->_frameSize);
    frame->_frameSize = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  T* data() const { return _buf; }

  size_t frameSize() const { return _frameSize; };

 private:
  size_t _frameSize;
  T* _buf;

  static const size_t defaultFrameSize = 64;
};

namespace IPC {
struct HexCharStruct {
  unsigned char c;
  explicit HexCharStruct(unsigned char _c) : c(_c) {}

  friend std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
  {
    return (o << std::hex << (int)hs.c);
  }

  static inline HexCharStruct to_hex(unsigned char _c)
  {
    return HexCharStruct(_c);
  }
};
template <typename T>
inline void print_mem(const T* pMem, size_t len)
{
  std::cout << "Memory block at:" << pMem << " with length:" << std::dec << len
            << '\n';
  for (auto i = 1; i < len; i++) {
    std::cout << HexCharStruct::to_hex(*((unsigned char*)pMem + i - 1));
    if (i % 4 == 0) std::cout << ' ';
    if (!(i % 16)) std::cout << '\n';
  }
  std::cout << '\n';
}
class Frame {
 public:
  enum class STATE {
    START,
    INITIALISED,
    READY_TO_WORK,
    READY_TO_READ,
    READY_TO_WRITE,
    END_SOURCE,
    DONE,
    TERMINATE
  };

  struct DataBulk {
    STATE _state{STATE::START};
    size_t _sizeOfData{0};
    byte _data_bulk[64];
  };

  using T = DataBulk;
  using TRef = T*;

  Frame()
  {
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,  // use paging file
        nullptr,               // default security
        PAGE_READWRITE,        // read/write access
        0,                     // maximum object size (high-order DWORD)
        defaultFrameSize,      // maximum object size (low-order DWORD)
        TEXT(szName));         // name of mapping object

    if (hMapFile == nullptr) {
      exit(2);  // TODO do some mechanism with errors
    }

    _buf = (TRef)MapViewOfFile(hMapFile,             // handle to map object
                               FILE_MAP_ALL_ACCESS,  // read/write permission
                               0, 0, defaultFrameSize);

    if (_buf == nullptr) {
      auto err =
          GetLastError();  // the same, need to save error and check after
      if (!err) {
        CloseHandle(hMapFile);
      }
      exit(3);
    }
    _old_handler = set_terminate([]() {
      HANDLE hMapFile =
          OpenFileMapping(FILE_MAP_ALL_ACCESS,  // read/write access
                          FALSE,                // do not inherit the name
                          TEXT(szName));        // name of mapping object
      if (hMapFile != NULL) {
        auto pMem =
            (TRef)MapViewOfFile(hMapFile,  // handle to file mapping object
                                FILE_MAP_ALL_ACCESS,  // read/write access
                                0, 0, defaultFrameSize);
        pMem->_state = STATE::TERMINATE;
        UnmapViewOfFile(pMem);
        CloseHandle(hMapFile);
      }
      std::cout << "Memory utilized. Sayanara!";
    });

    if (isState(STATE::START)) {
      new (_buf) T();
      setState(STATE::INITIALISED);
    }
    else if (isState(STATE::INITIALISED)) {
      setState(STATE::READY_TO_WORK);
    }
    else {
      exit(4);
    }
  }

  ~Frame()
  {
    set_terminate(_old_handler);
    UnmapViewOfFile(_buf);
    CloseHandle(hMapFile);
  }

  TRef data() const { return _buf; }

  bool isState(STATE state)
  {
    std::lock_guard<std::mutex> lock(_m);
    return _buf->_state == state;
  }

  void setState(STATE state)
  {
    std::lock_guard<std::mutex> lock(_m);
    // std::cout << "\nState " << static_cast<int>(_buf->_state) << " to "
    //           << static_cast<int>(state) << '\n';
    _buf->_state = state;
  }

  void logDataBulk() const
  {
#ifdef DEBUG
    std::cout << "Size of block " << _buf->_sizeOfData << "\nState "
              << static_cast<int>(_buf->_state) << "\nBulk data\n"
              << _buf->_data_bulk;
#endif
  }

  friend std::basic_istream<byte>& operator>>(std::basic_istream<byte>& ifs,
                                              Frame& frame)
  {
    ifs.read(frame._buf->_data_bulk, 64 - frame._buf->_sizeOfData);
    frame._buf->_sizeOfData = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  friend std::basic_istream<byte>& operator>>(std::basic_istream<byte>& ifs,
                                              Frame* frame)
  {
    ifs.read(frame->_buf->_data_bulk, 64 - frame->_buf->_sizeOfData);
    frame->_buf->_sizeOfData = static_cast<size_t>(ifs.gcount());
    return ifs;
  }

  friend std::basic_ostream<byte>& operator<<(std::basic_ostream<byte>& ofs,
                                              const Frame* frame)
  {
    ofs.write(frame->_buf->_data_bulk, frame->_buf->_sizeOfData);
    return ofs;
  }

  friend std::basic_ostream<byte>& operator<<(std::basic_ostream<byte>& ofs,
                                              const Frame& frame)
  {
    ofs.write(frame._buf->_data_bulk, frame._buf->_sizeOfData);
    return ofs;
  }

 private:
  Frame(const Frame*) = delete;
  Frame(Frame*) = delete;
  Frame& operator=(const Frame*) = delete;
  Frame(const Frame&&) = delete;
  Frame& operator=(const Frame&&) = delete;

 private:
  static inline const char szName[] = {"Global\\MyFileMappingObject"};
  static const size_t defaultFrameSize = sizeof(DataBulk);
  [[maybe_unused]] HANDLE hMapFile;
  std::terminate_handler _old_handler;
  std::mutex _m;
  TRef _buf;
};
}  // namespace IPC

}  // namespace Clone
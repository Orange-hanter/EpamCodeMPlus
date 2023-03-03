#pragma once
#include <filesystem>
#include <iostream>

namespace Clone::Utils {

namespace fs = std::filesystem;

inline fs::path ManipulateWithName(fs::path file_path, fs::path folder_path)
{
  // todo add handling cases when new file should be at the same folder and name
  // is not specified
  if (fs::is_regular_file(fs::status(folder_path))) {
    return folder_path;
  }
  return folder_path.append(fs::path(file_path).filename().string());
}

std::string ManipulateWithName(std::string file_path, std::string folder_path)
{
  return ManipulateWithName(fs::path(file_path), fs::path(folder_path))
      .string();
}
struct HexCharStruct {
  unsigned char c;
  explicit HexCharStruct(unsigned char _c) : c(_c) {}

  friend std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
  {
    return (o << std::hex << (int)hs.c);
  }

  static inline HexCharStruct to_hex(unsigned char _c) { return HexCharStruct(_c); }
};

template <typename T>
void print_mem(const T* pMem, size_t len)
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

void print_mem(const void* pMem, size_t len)
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
}  // namespace Clone::Utils
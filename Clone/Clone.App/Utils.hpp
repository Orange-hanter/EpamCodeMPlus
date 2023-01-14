#pragma once
#include <filesystem>
#include <iostream>


namespace Clone {

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
}  // namespace Clone

struct HexCharStruct {
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c) { return HexCharStruct(_c); }

void print_mem(void* pMem, size_t len)
{
  std::cout << "Memory block at:" << pMem << " with length:" << len << '\n';
  for (auto i = 1; i < len; i++) {
    std::cout << hex(*((unsigned char*)pMem + i - 1));
    if (i % 4 == 0) std::cout << ' ';
    if (!(i % 16)) std::cout << '\n';
  }
  std::cout << '\n';
}
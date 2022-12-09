#pragma once
#include <filesystem>

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
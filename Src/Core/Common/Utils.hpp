#ifndef EPAMMIDDLE_UTILS_HPP
#define EPAMMIDDLE_UTILS_HPP
#pragma once
#include <filesystem>
#include <iostream>

namespace Clone::Utils
{

namespace fs = std::filesystem;

inline fs::path ManipulateWithName(fs::path file_path, fs::path folder_path)
{
    // todo add handling cases when new file should be at the same folder and name
    // is not specified
    if (fs::is_regular_file(fs::status(folder_path)))
    {
        return folder_path;
    }
    return folder_path.append(fs::path(file_path).filename().string());
}

inline std::string ManipulateWithName(std::string file_path, std::string folder_path)
{
    return ManipulateWithName(fs::path(file_path), fs::path(folder_path)).string();
}

struct HexCharStruct
{
    unsigned char c;
    explicit HexCharStruct(unsigned char _c) : c(_c) {}

    friend std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) { return (o << std::hex << (int)hs.c); }

    static inline HexCharStruct to_hex(unsigned char _c) { return HexCharStruct(_c); }
};

template <typename T>
inline void print_mem(const T* pMem, size_t len)
{
    std::cout << "Memory block at:" << pMem << " with length:" << std::dec << len << '\n';
    for (auto i = 1; i < len; i++)
    {
        std::cout << HexCharStruct::to_hex(*((unsigned char*)pMem + i - 1));
        if (i % 4 == 0) std::cout << ' ';
        if (!(i % 16)) std::cout << '\n';
    }
    std::cout << '\n';
}

inline std::string calculateHash(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::hash<std::string> hasher;
    return std::to_string(hasher(buffer.str()));
}

inline std::string genTempFileName(std::string_view fileName)
{
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    ss << std::put_time(std::localtime(&time), "%Y%m%d%H%M%S");

    auto mlsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    ss << mlsec.count();
    ss << "_" << fileName;
    return ss.str();
}

template <class Type>
inline std::size_t streamSize(std::basic_ifstream<Type>& ifs)
{
    auto curPos = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    auto filesize = ifs.tellg();
    ifs.seekg(curPos);
    return filesize;
}

}  // namespace Clone::Utils
#endif  // EPAMMIDDLE_UTILS_HPP
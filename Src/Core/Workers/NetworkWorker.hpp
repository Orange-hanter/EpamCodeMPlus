#pragma once

#include <asio.hpp>

#include <list>
#include <utility>
#include <filesystem>

#include "iWorker.hpp"


namespace Clone::Workers {
namespace fs = std::filesystem;


/// @brief  NetworkWriter implement Server part. Keep connection and
///          save the data. Someone who write file to end destination
class NetworkWriter final : public IWriter {

  std::filesystem::path _repositoryPath;
  std::string _listeningPort;
  std::list<std::string> _downloadedList;

  std::condition_variable _cv;

 public:
  NetworkWriter(const std::string& repositoryPath, std::string listeningPort)
      : _repositoryPath(repositoryPath),
        _listeningPort(std::move(listeningPort))
  {
  }

  int execute() override { return 42; };

  bool isDone() override { return true; };

  void stopServer()
  {
    // TODO: Stop server and modify indicator that everything is stopped
  }

 private:
  /// @brief
  std::string genTempFileName(const std::string& fileName)
  {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    ss << std::put_time(std::localtime(&time), "%Y%m%d%H%M%S");

    auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()) %
                    1000;

    ss << millisec.count();
    ss << "_" << fileName;
    return ss.str();
  }

  std::string calculateHash(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
      return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::hash<std::string> hasher;
    return std::to_string(hasher(buffer.str()));
  }

  void copyFile(const fs::path& sourcePath, fs::path destinationPath) {
    std::ifstream sourceFile(sourcePath, std::ios::binary);
    std::ofstream destinationFile(destinationPath, std::ios::binary);

    if (sourceFile && destinationFile) {
      destinationFile << sourceFile.rdbuf();
      std::cout << "File copied successfully." << std::endl;
    } else {
      std::cout << "Failed to copy file." << std::endl;
    }
  }

  void processFile(const std::string& filePath, const std::string& directoryPath) {
    std::string fileName = fs::path(filePath).filename().string();
    std::string hash = calculateHash(filePath);

    fs::path destinationPath = fs::path(directoryPath) / fileName;
    if (fs::exists(destinationPath) && calculateHash(destinationPath.string()) == hash) {
      std::cout << "File with the same name and hash already exists. Skipping." << std::endl;
      return;
    }

    std::size_t underscorePos = fileName.find('_');
    if (underscorePos != std::string::npos) {
      fileName = fileName.substr(underscorePos + 1);
    }

    destinationPath = fs::path(directoryPath) / fileName;
    copyFile(filePath, destinationPath.string());
  }


};

/// @brief Role of reader is just Client like. Read source and send data
///         to some endpoint.
class NetworkReader final : public IReader {
  bool _done{false};
  const std::size_t _packageSize{4096};
  std::filesystem::path _filePath;
  std::string _serverPort;
  std::string _serverAddress;

 public:
  NetworkReader(std::string serverAddress, std::string serverPort,
                const std::string& filePath)
      : _filePath(filePath),
        _serverAddress(std::move(serverAddress)),
        _serverPort(std::move(serverPort))
  {
  }
  int execute() override { return 42; };
  bool isDone() override { return _done; };

 private:
};

}  // namespace Clone::Workers
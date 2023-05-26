#pragma once

#include <asio.hpp>
#include <utility>

#include "iWorker.hpp"

namespace Clone::Workers {

/// @brief  NetworkWriter implement Server part. Keep connection and
///          save the data. Someone who write file to end destination
class NetworkWriter final : public IWriter {
  asio::io_context _ioContext;
  asio::ip::tcp::acceptor* _acceptor{nullptr};

  std::filesystem::path _repositoryPath;
  std::string _listeningPort;

 public:
  NetworkWriter(const std::string& repositoryPath, std::string listeningPort)
      : _repositoryPath(repositoryPath),
        _listeningPort(std::move(listeningPort))
  {
  }

  int execute() override
  {
    receiveFile();
    return 42;
  };

  bool isDone() override { return true; };

 private:
  void start()
  {
    using asio::ip::tcp;
    _acceptor = new tcp::acceptor(
        _ioContext, tcp::endpoint(tcp::v4(), std::stoi(_listeningPort)));
  }

  void receiveFile()
  {
    using asio::ip::tcp;
    try {
      // Create an acceptor to listen for incoming connections

      std::cout << "Waiting for incoming connection..." << std::endl;

      // Accept a new connection
      tcp::socket socket(_ioContext);
      _acceptor->accept(socket);

      std::string filename = genTempFileName();
      // Open the file to save the received data
      std::ofstream file(filename, std::ios::binary);
      if (!file) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return;
      }

      // Receive and save the data
      std::vector<char> buffer(4096);
      std::size_t bytesRead = 0;
      while ((bytesRead = socket.read_some(asio::buffer(buffer))) > 0) {
        file.write(buffer.data(), bytesRead);
      }

      // Close the file and the socket
      file.close();
      socket.close();

      std::cout << "File transfer complete. Received file: " << filename
                << std::endl;
    }
    catch (const std::exception& ex) {
      std::cerr << "Exception: " << ex.what() << std::endl;
    }
  }

  /// @brief
  std::string genTempFileName() { return {"fileName"}; }
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
  int execute() override
  {
    sendFile();
    return 42;
  };
  bool isDone() override { return _done; };

 private:
  void sendFile()
  {
    using asio::ip::tcp;
    try {
      asio::io_context ioContext;

      // Resolve the server address and port
      tcp::resolver resolver(ioContext);
      tcp::resolver::results_type endpoints =
          resolver.resolve(_serverAddress, _serverPort);

      // Create a socket and connect to the server
      tcp::socket socket(ioContext);
      asio::connect(socket, endpoints);

      // Open the file to send
      std::ifstream file(_filePath, std::ios::binary);
      if (!file) {
        std::cerr << "Failed to open file: " << _filePath << std::endl;
        _done = true;
        return;
      }

      // Read and send the file data
      std::vector<char> buffer(_packageSize);
      while (file.read(buffer.data(), buffer.size())) {
        asio::write(socket, asio::buffer(buffer.data(), file.gcount()));
      }

      // Close the file and the socket
      file.close();
      socket.close();

      std::cout << "File transfer complete." << std::endl;
    }
    catch (const std::exception& ex) {
      std::cerr << "Exception: " << ex.what() << std::endl;
    }
  }
};

}  // namespace Clone::Workers
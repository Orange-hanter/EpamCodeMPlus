#pragma once

#include <asio.hpp>
#include <list>
#include <utility>

#include "TCPAcceptor.hpp"
#include "Uploader.hpp"
#include "iWorker.hpp"

namespace Clone::Workers
{
namespace fs = std::filesystem;

using TransportLayer::TCPAcceptor;

/// @brief  NetworkWriter implement Server part. Keep connection and
///          save the data. Someone who write file to end destination
class NetworkWriter final : public IWriter
{
    using HASH = std::string;

public:
    NetworkWriter(const std::string& repositoryPath, const std::string& listeningPort)
        : _repositoryPath(repositoryPath),
          m_acceptor_ptr(std::move(std::make_unique<TCPAcceptor>(std::stoi(listeningPort))))
    {
    }

    int execute() override
    {
        // TODO: start here handler that monitor queue of downloaded files
        return 0;
    };

    bool isDone() override { return false; };

    void waitTillDone() {}

    void stopServer()
    {
        // TODO: Stop server and modify indicator that everything is stopped
    }

private:
    void file_handler(fs::path newFile /*maybe need change for some file-descrioptor*/)
    {
        /*TODO:
         * 1. if this handler was called, new file was downloaded
         * 2. calc or get data from file-descriptor and compare with DB(need to implement)
         * 3. if file exist and hash equal - skip next processing
         * 4. else put new file to repo with name {datetime#downloaded_file_name}
         * */
    }

    void copyFile(const fs::path& sourcePath, const fs::path& destinationPath)
    {
        std::ifstream sourceFile(sourcePath, std::ios::binary);
        std::ofstream destinationFile(destinationPath, std::ios::binary);

        if (sourceFile && destinationFile)
        {
            destinationFile << sourceFile.rdbuf();
            std::cout << "File copied successfully." << std::endl;
        }
        else
        {
            std::cout << "Failed to copy file." << std::endl;
        }
    }

    std::string calculateHash(fs::path);

    void processFile(const std::string& filePath, const std::string& directoryPath)
    {
        std::string fileName     = fs::path(filePath).filename().string();
        std::string hash         = calculateHash(filePath);

        fs::path destinationPath = fs::path(directoryPath) / fileName;
        if (fs::exists(destinationPath) && calculateHash(destinationPath.string()) == hash)
        {
            std::cout << "File with the same name and hash already exists. Skipping." << std::endl;
            return;
        }

        std::size_t underscorePos = fileName.find('_');
        if (underscorePos != std::string::npos)
        {
            fileName = fileName.substr(underscorePos + 1);
        }

        destinationPath = fs::path(directoryPath) / fileName;
        copyFile(filePath, destinationPath.string());
    }

    void serializeRepository()
    {
        // TODO: chash data saved in map to improve speed
        std::cerr << "Serialization repository isn't implemented!\n";
    }

private:
    std::condition_variable w8;
    std::mutex cv_m;

    std::filesystem::path _repositoryPath;
    std::list<std::string> _downloadedList;

    std::unique_ptr<TCPAcceptor> m_acceptor_ptr{nullptr};
    std::map<HASH, ::fs::path> m_repository;
};

/// @brief Role of reader is just Client like. Read source and send data
///         to some endpoint.
class NetworkReader final : public IReader
{
public:
    NetworkReader(std::string serverAddress, std::string serverPort, const std::string& filePath)
        : _filePath(filePath), _serverAddress(std::move(serverAddress)), _serverPort(std::move(serverPort)),
          _uploaderPtr(std::make_shared<TransportLayer::Uploader>(_serverAddress, _serverPort, ::fs::path(filePath)))
    {}

    int execute() override
    {
        _uploaderPtr->doConnect();
        return 0;
    };

    bool isDone() override { return !_uploaderPtr->isConnected(); };

private:
    const std::size_t _packageSize{4096};
    std::filesystem::path _filePath;
    std::string _serverPort;
    std::string _serverAddress;
    std::shared_ptr<TransportLayer::Uploader>  _uploaderPtr;
};

}  // namespace Clone::Workers
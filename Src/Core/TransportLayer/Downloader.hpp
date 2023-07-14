//
// Created by daniil on 6/1/23.
//

#ifndef EPAMMIDDLE_DOWNLOADER_HPP
#define EPAMMIDDLE_DOWNLOADER_HPP

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <utility>

using asio::ip::tcp;

#include "messages.pb.h"

namespace Clone::TransportLayer
{

class DLManager;
class Downloader;
using Downloader_ptr  = std::shared_ptr<Downloader>;
using DefaultBuffer_t = std::array<char, 1024>;

class Downloader : public std::enable_shared_from_this<Downloader>
{
    void doReadNewRequest();
    void requestProcessing();
    void doDownload();
    void assembleFile();

    void sendResponse(Filetransfer::ReturnCode code, std::string message = "");

    template <typename Handler_t, typename... Args>
    void doWrite(const google::protobuf::MessageLite* request, Handler_t handler, Args... args);
    void doWrite(const google::protobuf::MessageLite* request);
    void doWrite(std::shared_ptr<google::protobuf::MessageLite>&& request);

    template <typename Handler_t>
    void doRead(Handler_t handler);

    template <typename Handler_t>
    void doReadS(Handler_t handler);

public:
    Downloader(asio::io_context& ctx, tcp::socket& socket, DLManager& mngr, fs::path dfPath = "/tmp/");

    ~Downloader();

    void start();

    tcp::socket& socket() { return m_socket; }

private:
    asio::io_context& m_context;
    tcp::socket m_socket;
    DLManager& m_dlManager;
    std::array<char, 4096> m_data{};
    fs::path m_defaultFilePath;
    std::unique_ptr<Filetransfer::FileTransferRequest_t> m_candidate;
    std::map<int, std::string> memory{};
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_DOWNLOADER_HPP

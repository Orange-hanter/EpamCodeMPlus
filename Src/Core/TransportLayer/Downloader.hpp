//
// Created by daniil on 6/1/23.
//

#ifndef EPAMMIDDLE_DOWNLOADER_HPP
#define EPAMMIDDLE_DOWNLOADER_HPP

#include <filesystem>
#include <memory>
#include <concepts>

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
using Downloader_ptr = std::shared_ptr<Downloader>;

template <typename T>
concept ProtobufMessageDerived = std::derived_from<T, ::google::protobuf::Message>;

class Downloader : public std::enable_shared_from_this<Downloader>
{
    void doReadNewRequest();

    void requestValidation(Filetransfer::FileTransferRequest_t* request);

    void doReadFile();

    template <ProtobufMessageDerived ProtobufMessage>
    ProtobufMessage* parseProtobufMsg();

public:
    explicit Downloader(asio::io_context& ctx, tcp::socket& socket, DLManager& mngr, fs::path dfPath = "./tmp/")
        : m_context(ctx), m_socket(std::move(socket)), m_dlManager(mngr), m_defaultFilePath(std::move(dfPath))
    {
        std::cout << "Connection created" << &(*this) << '\n';
    }

    ~Downloader() { std::cout << "Connection closed" << &(*this) << '\n'; }

    void start();

    tcp::socket& socket() { return m_socket; }

private:
    asio::io_context& m_context;
    tcp::socket m_socket;
    DLManager& m_dlManager;
    std::array<char, 1024> m_data{};
    fs::path m_defaultFilePath;
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_DOWNLOADER_HPP

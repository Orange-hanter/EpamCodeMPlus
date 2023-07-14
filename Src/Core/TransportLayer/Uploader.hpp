//
// Created by daniil on 6/8/23.
//

#ifndef EPAMMIDDLE_UPLOADER_HPP
#define EPAMMIDDLE_UPLOADER_HPP

#include <concepts>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

#include <asio/streambuf.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <utility>

using asio::ip::tcp;

#include <google/protobuf/message_lite.h>

namespace Clone::TransportLayer
{

class Uploader : public std::enable_shared_from_this<Uploader>
{
    void doCloseSession();
    void doInitTransmission();
    void doReadResponse();
    void doTransferFile();

    void doWrite(const google::protobuf::MessageLite* request, void (Uploader::*handler)(), std::uint32_t flags = 0);
    void doWrite(const google::protobuf::MessageLite* request, std::uint32_t flags = 0);

    template <typename Handler_t>
    void doRead(Handler_t handler);

public:
    Uploader(std::string_view destination, std::string_view port, fs::path file);
    ~Uploader();

    void doConnect();
    bool isConnected() const;

private:
    asio::io_context m_context{};
    tcp::socket m_socket;
    tcp::resolver::results_type m_endpoint;

    enum ConnectionState
    {
        CLOSED          = 0,
        WAIT_CONNECTION = 1,
        CONNECTED       = 2
    } m_connectionState{ConnectionState::CLOSED};

    static constexpr std::size_t m_chunkSize{512};
    std::array<char, m_chunkSize * 2> m_data{};
    fs::path m_FilePath;
};
}  // namespace Clone::TransportLayer
#endif  // EPAMMIDDLE_UPLOADER_HPP

//
// Created by daniil on 6/8/23.
//

#ifndef EPAMMIDDLE_UPLOADER_HPP
#define EPAMMIDDLE_UPLOADER_HPP

#include <concepts>
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
class Uploader : public std::enable_shared_from_this<Uploader>
{
    void doCloseConnection();
    void doInitTransmission();
    void doReadResponse();

    void doTransferFile(){};

 public:
    Uploader(std::string_view destination, std::string_view port, fs::path file);
    ~Uploader();

    void doConnect();

private:
    asio::io_context m_context{};
    tcp::socket m_socket;
    tcp::resolver::results_type m_endpoint;

    std::array<char, 1024> m_data{};
    fs::path m_FilePath;
};
}  // namespace Clone::TransportLayer
#endif  // EPAMMIDDLE_UPLOADER_HPP

//
// Created by daniil on 6/8/23.
//

#include "Uploader.hpp"

#include <asio/connect.hpp>
#include <asio/streambuf.hpp>
#include <charconv>
#include <thread>
#include <utility>

#include "messages.pb.h"

namespace Clone::TransportLayer
{

void Uploader::doInitTransmission()
{
    Filetransfer::FileTransferRequest request;
    request.set_file_name("filename");
    request.set_file_size_kb(1234);
    request.set_md5_hash("asdasfcxvef");


    std::string buf;
    request.SerializeToArray();
    auto bb = asio::buffer(buf, buf.size());
    asio::async_write(m_socket, bb, [this](std::error_code ec, std::size_t) {
        if (!ec) doReadResponse();
    });
}

Uploader::Uploader(std::string_view destination, std::string_view port, fs::path file) : m_socket(m_context), m_FilePath(std::move(file))
{
    tcp::resolver resolver(m_context);
    int _port;
    std::from_chars(port.begin(), port.end(), _port);
    m_endpoint = resolver.resolve(destination, port);

}

void Uploader::doConnect()
{
    asio::async_connect(m_socket, m_endpoint, [this](const std::error_code& ec, const tcp::endpoint& ep) {
        std::cout << ep << '\n';
        if (!ec)
        {
            doInitTransmission();
        }
    });
    std::thread([this]() { m_context.run(); }).detach();
}

void Uploader::doCloseConnection()
{
    asio::post(m_context, [this]() { m_socket.close(); });
}

Uploader::~Uploader() { doCloseConnection(); }

void Uploader::doReadResponse()
{
    asio::streambuf buf;
    asio::async_read(m_socket, buf, [this, &buf](std::error_code ec, std::size_t received) {
        if (!ec)
        {
            std::istream out(&buf);
            std::string str;
            out >> str;
            std::cout << str;
        }
        else
        {
            std::cout << asio::system_error(ec).what() << "\n>> " << std::endl;
        }
    });
}

}  // namespace Clone::TransportLayer

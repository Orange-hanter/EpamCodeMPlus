//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_TCPTRANSPORTLAYER_HPP
#define EPAMMIDDLE_TCPTRANSPORTLAYER_HPP

#include <asio.hpp>

#include "iTransportLayer.hpp"

namespace Clone::TransportLayer {

namespace {
using asio::ip::tcp;
}

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
 public:
  using pointer = std::shared_ptr<tcp_connection>;

  static pointer create(asio::io_context& io_context)
  {
    return pointer(new tcp_connection(io_context));
  }

  tcp::socket& socket() { return socket_; }

  void start()
  {
    auto error_ph = std::placeholders::_1;
    auto bytes_transferred_ph = std::placeholders::_2;
    // TODO: after start here should be somesing like reading file and sendint
    asio::async_write(
        socket_, asio::buffer(message_),
        std::bind(&tcp_connection::handle_write, shared_from_this(), error_ph,
                  bytes_transferred_ph));
  }

 private:
  explicit tcp_connection(asio::io_context& io_context) : socket_(io_context) {}

  void handle_write(const asio::error_code& /*error*/,
                    size_t /*bytes_transferred*/)
  {
  }

  tcp::socket socket_;
  std::string message_;
};

class TCPAcceptor : public iTransportLayer {
  asio::io_context _ioContext;
  std::unique_ptr<tcp::acceptor> _acceptor;

  void start_accept()
  {
    auto new_connection = tcp_connection::create(_ioContext);

    _acceptor->async_accept(
        new_connection->socket(),
        std::bind(&TCPAcceptor::handle_accept, this, new_connection,
                  std::placeholders::_1));
  }

  void handle_accept(const tcp_connection::pointer& new_connection,
                     const asio::error_code& error)
  {
    if (!error) {
      new_connection->start();
    }

    start_accept();
  }

 public:
  explicit TCPAcceptor(int listeningPort = 8743)
  {
    _acceptor = std::make_unique<tcp::acceptor>(
        _ioContext, tcp::endpoint(tcp::v4(), listeningPort));
  }

  void read() override{};

  void write() override{};
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_TCPTRANSPORTLAYER_HPP

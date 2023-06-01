//
// Created by daniil on 6/1/23.
//

#ifndef EPAMMIDDLE_CONNECTION_HPP
#define EPAMMIDDLE_CONNECTION_HPP

#include <memory>
#include <asio.hpp>

using asio::ip::tcp;

class Connection {};


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

#endif  // EPAMMIDDLE_CONNECTION_HPP

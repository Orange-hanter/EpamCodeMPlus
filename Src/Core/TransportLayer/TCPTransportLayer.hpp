//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_TCPTRANSPORTLAYER_HPP
#define EPAMMIDDLE_TCPTRANSPORTLAYER_HPP

#include <asio.hpp>
#include <functional>

#include "DlManager.hpp"

namespace Clone::TransportLayer {

using asio::ip::tcp;

class TCPAcceptor {

  void start_accept()
  {
    auto newSession = std::make_shared<Downloader>(_ioContext, _manager_ptr);
    auto error = std::placeholders::_1;
    _acceptor->async_accept(newSession->socket(),
        std::bind(&TCPAcceptor::handle_accept, this, error, std::move(newSession))
        );
  }

  void handle_accept(const asio::error_code& error, Downloader_ptr new_connection)
  {
    // TODO: not shure that extra call won't harm performance
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
    start_accept();
  }

 private:
  asio::io_context _ioContext;
  std::unique_ptr<tcp::acceptor> _acceptor;
  DLManager _manager_ptr;
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_TCPTRANSPORTLAYER_HPP

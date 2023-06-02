//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_TCPTRANSPORTLAYER_HPP
#define EPAMMIDDLE_TCPTRANSPORTLAYER_HPP

#include <asio.hpp>

#include <set>

#include "iTransportLayer.hpp"
#include "Downloader.hpp"

namespace Clone::TransportLayer {

using asio::ip::tcp;



// crtp

class TCPAcceptor {

  void start_accept()
  {
    auto newSession = std::make_shared<Downloader>(_ioContext);
    _DLManager.insert(newSession);
    using error = std::placeholders::_1;
    _acceptor->async_accept(newSession->socket(),
        std::bind(&TCPAcceptor::handle_accept, this, error, newSession)
        );
  }

  void handle_accept(const asio::error_code& error, const Downloader& new_connection,)
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
  std::set<std::shared_ptr<Downloader>> _DLManager;
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_TCPTRANSPORTLAYER_HPP

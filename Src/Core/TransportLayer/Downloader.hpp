//
// Created by daniil on 6/1/23.
//

#ifndef EPAMMIDDLE_DOWNLOADER_HPP
#define EPAMMIDDLE_DOWNLOADER_HPP

#include <memory>

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

using asio::ip::tcp;

namespace Clone::TransportLayer {

class Downloader : public std::enable_shared_from_this<Downloader> {
 public:
  Downloader(asio::io_context ctx): m_context(ctx)
  {}

  private:
   asio::io_context& m_context;
   tcp::socket socket_;
};

} /// End Clone::TransportLayer

#endif  // EPAMMIDDLE_DOWNLOADER_HPP

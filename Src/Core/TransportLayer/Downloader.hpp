//
// Created by daniil on 6/1/23.
//

#ifndef EPAMMIDDLE_DOWNLOADER_HPP
#define EPAMMIDDLE_DOWNLOADER_HPP

#include <memory>
#include <filesystem>

using fs = std::filesystem;

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include "messages.pb.h"

using asio::ip::tcp;

namespace Clone::TransportLayer {


class Downloader : public std::enable_shared_from_this<Downloader> {

  void do_read_header()
  {
   
  }

 public:
  explicit Downloader(asio::io_context ctx): m_context(ctx), m_socket(ctx)
  {}

  void start()
  {
   do_read_header();
  }

  tcp::socket& socket() { return socket_; }

  private:
   asio::io_context& m_context;
   tcp::socket m_socket;
   fs::path m_tmp_file;
};

} /// End Clone::TransportLayer

#endif  // EPAMMIDDLE_DOWNLOADER_HPP

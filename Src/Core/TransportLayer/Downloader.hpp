//
// Created by daniil on 6/1/23.
//

#ifndef EPAMMIDDLE_DOWNLOADER_HPP
#define EPAMMIDDLE_DOWNLOADER_HPP

#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <utility>

using asio::ip::tcp;

#include "messages.pb.h"

namespace Clone::TransportLayer {

class DLManager;
class Downloader;
using Downloader_ptr = std::shared_ptr<Downloader>;

class Downloader : public std::enable_shared_from_this<Downloader> {
  void doReadNewRequest();

  void requestValidation(Filetransfer::FileTransferRequest request);

  void doReadFile();

 public:
  explicit Downloader(asio::io_context ctx, DLManager& mngr,
                      fs::path dfPath = "./tmp/")
      : m_context(ctx),
        m_socket(ctx),
        m_dlManager(mngr),
        m_defaultFilePath(std::move(dfPath))
  {
  }

  void start();

  tcp::socket& socket() { return m_socket; }

 private:
  asio::io_context& m_context;
  tcp::socket m_socket;
  DLManager& m_dlManager;
  enum {
    max_length = 1024
  };
  char m_data[max_length]{};
  fs::path m_defaultFilePath;
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_DOWNLOADER_HPP

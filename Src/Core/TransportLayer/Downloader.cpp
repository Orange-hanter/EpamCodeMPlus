//
// Created by daniil on 6/2/23.
//

#include "Downloader.hpp"
#include "DlManager.hpp"

#include <fstream>


namespace Clone::TransportLayer {

void Downloader::doReadNewRequest()
{
  // TODO: replace by protobuf message TransferRequest
  auto self = (shared_from_this());
  auto buf = asio::buffer(m_data, max_length);
  asio::async_read(
      m_socket, buf, [self](std::error_code ec, std::size_t length) {
        // TODO: lets imagine TransferRequest was read
        if (!ec) {
          Filetransfer::FileTransferRequest request;
          // TODO: investigate the proper way catching from buffer
          request.ParseFromString(std::string(self->m_data, length));
          self->requestValidation(request);
        }
        // TODO: else remove pointer from manager
        else {
          self->m_dlManager.stopSession(self);
        }
      });
}

void Downloader::requestValidation(Filetransfer::FileTransferRequest request)
{
  auto name = request.file_name();
  fs::path tmp_file_path = m_defaultFilePath.string() + '/' + name;
  /*TODO:
   * 1. make new promise from request data
   * 2. send request for
   * */
}

void Downloader::doReadFile() {

}

std::string genTempFileName(const std::string& fileName)
{
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;

  ss << std::put_time(std::localtime(&time), "%Y%m%d%H%M%S");

  auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

  ss << millisec.count();
  ss << "_" << fileName;
  return ss.str();
}

std::string calculateHash(const std::string& filePath) {
  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  std::hash<std::string> hasher;
  return std::to_string(hasher(buffer.str()));
}

void Downloader::start()
{
  m_dlManager.registerSession(shared_from_this());
  doReadNewRequest();
}

}  // namespace Clone::TransportLayer
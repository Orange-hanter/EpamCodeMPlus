//
// Created by daniil on 6/2/23.
//

#include "Downloader.hpp"

#include <fstream>
#include <asio/error_code.hpp>
#include <asio/streambuf.hpp>
#include "DlManager.hpp"

namespace Clone::TransportLayer
{

void Downloader::doReadNewRequest()
{
    // TODO: replace by protobuf message TransferRequest
    auto self = (shared_from_this());
    asio::streambuf buf;
    asio::async_read(self->socket(), asio::buffer(m_data, 5), [self](std::error_code ec, std::size_t length) {

        // TODO: lets imagine TransferRequest was read
        if (!ec)
        {
            for(auto c : self->m_data)
                std::cout << c;
            auto request = self->parseProtobufMsg<Filetransfer::FileTransferRequest>();
            if (request)
                self->requestValidation(request);
            std::cerr << "ERROR: File wasn't received\n";
            self->doReadNewRequest();
        }
        else
        {
            std::cout << asio::system_error(ec).what()
                      << "\n>> "
                      << self->m_data.data()
                      << std::endl;

            self->m_dlManager.stopSession(self);
        }
    });
}

void Downloader::requestValidation(Filetransfer::FileTransferRequest* request)
{
    auto name              = request->file_name();
    fs::path tmp_file_path = m_defaultFilePath.string() + '/' + name;
    /*TODO:
     * 1. make new promise from request data
     * 2. send request for
     * */
    delete request;

    Filetransfer::FileTransferResponse response;
    response.set_error_message("");
    response.set_return_code(Filetransfer::ReturnCode::OK);

    std::string buf;
    response.SerializeToString(&buf);
    auto bb = asio::buffer(buf, buf.size());
    asio::async_write(m_socket, bb, [](std::error_code, std::size_t){});
}

std::string genTempFileName(const std::string& fileName)
{
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;

    ss << std::put_time(std::localtime(&time), "%Y%m%d%H%M%S");

    auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    ss << millisec.count();
    ss << "_" << fileName;
    return ss.str();
}

std::string calculateHash(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
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

template <ProtobufMessageDerived ProtobufMessage>
ProtobufMessage*  Downloader::parseProtobufMsg()
{
    auto msg_p = new ProtobufMessage();
    if (msg_p->ParseFromArray(m_data.data(), m_data.size())){
        return  msg_p;
    }
    return {nullptr};
}

}  // namespace Clone::TransportLayer
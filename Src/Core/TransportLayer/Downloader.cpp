//
// Created by daniil on 6/2/23.
//

#include "Downloader.hpp"

#include <asio/streambuf.hpp>
#include <fstream>
#include <future>
#include <source_location>

#include "DlManager.hpp"
#include "MessageUtils.hpp"
#include "Utils.hpp"

namespace Clone::TransportLayer
{

void Downloader::doReadNewRequest()
{
    auto self = (shared_from_this());
    doRead([self]() {
        auto request = Utils::parseProtobufMsg<Filetransfer::FileTransferRequest_t, decltype(self->m_data)>(self->m_data);

        if (request)
            self->requestValidation(request.get());
        else
        {
            std::cerr << "ERROR: File wasn't received correctly\n";
            return;  // ToDo close connection??
        }
    });

}

void Downloader::requestValidation(const Filetransfer::FileTransferRequest_t* request)
{
    auto name              = request->file_name();
    fs::path tmp_file_path = m_defaultFilePath.append(name);  // TODO check if file already exist
    /*TODO:
     * 1. make new promise from request data
     * 2. send request for
     * */

    Filetransfer::FileTransferResponse_t response;
    response.set_return_code(Filetransfer::ReturnCode::OK);

    doWrite(&response, &Downloader::doDownload);
}

void Downloader::doDownload()
{
    std::map<int, std::string> memory{};
    auto self = shared_from_this();

    while (true)
    {
        m_data.fill(0);
        doReadS([self, &memory]() {
            auto request = Utils::parseProtobufMsg<Filetransfer::FileChunk_t, decltype(self->m_data)>(self->m_data);
            if (!request) { std::cout << "Auch"; return ;}
            memory.emplace(request->sequence_num(), request->data());
            std::cout << memory.size() << '\n';
            //TODO merge file from parts
        });
    };
}

void Downloader::doWrite(const google::protobuf::MessageLite* request, void (Downloader::*handler)())
{
    auto buf = Utils::MessageToVector(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [this, handler](std::error_code ec, std::size_t) {
        if (!ec) (this->*handler)();
    });
}

void Downloader::doWrite(const google::protobuf::MessageLite* request)
{
    auto buf = Utils::MessageToVector(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [](std::error_code ec, std::size_t) {
        if (ec) std::cerr << std::source_location::current().line() << ": " << ec;
    });
}

template <typename Handler_t>
void Downloader::doRead(Handler_t handler)
{
    auto self = (shared_from_this());
    asio::async_read(
        self->socket(), asio::buffer(m_data),
        [self](const std::error_code& ec, std::size_t received) {
            auto header_size{sizeof(std::uint32_t) + sizeof(std::uint64_t)};

            if (received < header_size) return header_size;

            std::uint32_t start{*reinterpret_cast<std::uint32_t*>(self->m_data.data())};
            if (start == Utils::start_dword)
            {
                auto message_size   = *reinterpret_cast<std::uint64_t*>(self->m_data.data() + sizeof(std::uint32_t));
                auto estimated_size = header_size + message_size + sizeof(Utils::end_dword);
                std::uint32_t end{*reinterpret_cast<std::uint32_t*>(self->m_data.data() + estimated_size - sizeof(Utils::end_dword))};
                if (end == Utils::end_dword) return 0UL;

                return estimated_size;
            }

            return 0UL;
        },
        [self, handler](std::error_code ec, std::size_t length) {
            if (!ec)
                handler();
            else if (ec == asio::error::eof)
            {
                ;  // end of connection
            }
        });
}

template <typename Handler_t>
void Downloader::doReadS(Handler_t handler)
{
    auto self = (shared_from_this());
    std::error_code ec{};
    asio::read(
        self->socket(), asio::buffer(m_data),
        [self](const std::error_code& ec, std::size_t received) {
            auto header_size{sizeof(std::uint32_t) + sizeof(std::uint64_t)};

            if (received < header_size) return header_size;

            std::uint32_t start{*reinterpret_cast<std::uint32_t*>(self->m_data.data())};
            if (start == Utils::start_dword)
            {
                auto message_size   = *reinterpret_cast<std::uint64_t*>(self->m_data.data() + sizeof(std::uint32_t));
                auto estimated_size = message_size + sizeof(Utils::end_dword);
                auto end_ptr_shift = header_size + message_size;
                std::uint32_t end{*reinterpret_cast<std::uint32_t*>(self->m_data.data() + end_ptr_shift)};
                if (end == Utils::end_dword) return 0UL;

                return estimated_size;
            }
            std::cout << "Error reading\n" << self->m_data.data() << std::endl;
            return 0UL;
        },
        ec);

    if (!ec)
        handler();
    else if (ec == asio::error::eof)
    {
        ;  // end of connection
    }
}

Downloader::Downloader(asio::io_context& ctx, tcp::socket& socket, DLManager& mngr, fs::path dfPath)
    : m_context(ctx), m_socket(std::move(socket)), m_dlManager(mngr), m_defaultFilePath(std::move(dfPath))
{
    std::cout << "Connection created" << &(*this) << '\n';
}

Downloader::~Downloader() { std::cout << "Connection closed" << &(*this) << '\n'; }

void Downloader::start()
{
    m_dlManager.registerSession(shared_from_this());
    doReadNewRequest();
}

}  // namespace Clone::TransportLayer
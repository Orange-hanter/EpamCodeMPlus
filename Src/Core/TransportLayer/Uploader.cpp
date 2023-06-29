//
// Created by daniil on 6/8/23.
//

#include "Uploader.hpp"

#include <asio/connect.hpp>
#include <asio/signal_set.hpp>
#include <charconv>
#include <fstream>
#include <source_location>
#include <thread>
#include <utility>

#include "Common/Utils.hpp"
#include "MessageUtils.hpp"
#include "messages.pb.h"

namespace Clone::TransportLayer
{

Uploader::Uploader(std::string_view destination, std::string_view port, fs::path file) : m_socket(m_context), m_FilePath(std::move(file))
{
    tcp::resolver resolver(m_context);
    int _port;
    std::from_chars(port.begin(), port.end(), _port);
    m_endpoint = resolver.resolve(destination, port);
}

void Uploader::doConnect()
{
    m_connectionState = ConnectionState::WAIT_CONNECTION;
    asio::async_connect(m_socket, m_endpoint, [this](const std::error_code& ec, const tcp::endpoint& ep) {
        std::cout << ep << '\n';  // TODO clear that
        if (!ec)
        {
            m_connectionState = ConnectionState::CONNECTED;
            doInitTransmission();
        }
    });
    std::thread([this]() {
        // TODO not sure that this is good idea
        asio::signal_set signals(m_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) { std::cout << "TERMINATE"; m_context.stop(); });
        m_context.run();
    }).detach();
}

bool Uploader::isConnected() const { return m_connectionState & (ConnectionState::WAIT_CONNECTION | ConnectionState::CONNECTED); }

void Uploader::doCloseSession()
{
    asio::post(m_context, [this]() { m_socket.close(); });
    m_context.stop();
    m_connectionState = ConnectionState::CLOSED;
}

Uploader::~Uploader() { doCloseSession(); }

void Uploader::doInitTransmission()
{
    Filetransfer::FileTransferRequest_t request;
    request.set_file_name(m_FilePath.filename().string());
    request.set_file_size_kb(std::filesystem::file_size(m_FilePath));
    request.set_md5_hash(Utils::calculateHash(m_FilePath));

    this->doWrite(&request, &Uploader::doReadResponse);
}

void Uploader::doReadResponse()
{
    doRead([this]() {
        auto rc = Utils::parseProtobufMsg<Filetransfer::FileTransferResponse_t, decltype(this->m_data)>(m_data);
        switch (rc->return_code())
        {
            case Filetransfer::OK:
                doTransferFile();
                break;
            case Filetransfer::ERROR:
                std::cerr << rc->error_message() << std::endl;
                doCloseSession();
                return;
                //TODO add response RECEIVED
            default:
                std::cerr << "Missed return code\n";
        }
    });
}

void Uploader::doTransferFile()
{
    // TODO complete
    auto ifs = std::ifstream(m_FilePath, std::ios::binary);
    static std::uint32_t chunk_N{0};
    while (!ifs.eof())
    {
        auto chunk = std::make_shared<Filetransfer::FileChunk_t>();
        auto reservSize {Utils::streamSize(ifs)};
        reservSize = (reservSize > m_chunkSize) ? m_chunkSize : reservSize;
        std::vector<char> buf(reservSize);

        ifs.read(buf.data(), buf.size());

        chunk->set_data(buf.data(), buf.size());
        chunk->set_sequence_num(chunk_N++);

        doWrite(chunk.get());
    }
}

void Uploader::doWrite(const google::protobuf::MessageLite* request, void (Uploader::*handler)())
{
    auto buf = Utils::MessageToVector(*request); // TODO here potentially memory deleeting faster than
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [this, handler, buf](std::error_code ec, std::size_t) {
        if (!ec)
        {
            (this->*handler)();
        }
        else
            std::cerr << std::source_location::current().line() << ": " << asio::system_error(ec).what() << "\n>> " << std::endl;
    });
}

void Uploader::doWrite(const google::protobuf::MessageLite* request)
{
    auto buf = Utils::MessageToVector(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [buf](std::error_code ec, std::size_t) {
        if (ec) std::cerr << std::source_location::current().line() << ": " << asio::system_error(ec).what() << "\n>> " << std::endl;
    });
}

template <typename Handler_t>
void Uploader::doRead(Handler_t handler)
{
    auto self = shared_from_this();
    asio::async_read(
        self->m_socket, asio::buffer(m_data),
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
        [self, handler](std::error_code ec, std::size_t length) {
            if (!ec)
                handler();
            else
                std::cerr << asio::system_error(ec).what() << "\n>> " << std::endl;
        });
}

}  // namespace Clone::TransportLayer

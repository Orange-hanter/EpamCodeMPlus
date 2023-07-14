//
// Created by daniil on 6/8/23.
//

#include "Uploader.hpp"

#include <asio/connect.hpp>
#include <asio/signal_set.hpp>
#include <boost/log/trivial.hpp>
#include <charconv>
#include <fstream>
#include <thread>
#include <utility>

#include "Common/Utils.hpp"
#include "MessageUtils.hpp"
#include "messages.pb.h"

namespace logging = boost::log;
using namespace std::chrono_literals;

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
        if (!ec)
        {
            BOOST_LOG_TRIVIAL(info) << "Connection establish";
            m_connectionState = ConnectionState::CONNECTED;
            doInitTransmission();
        }
    });
    std::thread([this]() {
        // TODO add signal processing
        asio::signal_set signals(m_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            BOOST_LOG_TRIVIAL(info) << "Termination session after external signal";
            m_context.stop();
        });
        m_context.run();
    }).detach();
}

bool Uploader::isConnected() const { return m_connectionState & (ConnectionState::WAIT_CONNECTION | ConnectionState::CONNECTED); }

void Uploader::doCloseSession()
{
    BOOST_LOG_TRIVIAL(info) << "Normally closing session";
    asio::post(m_context, [this]() { m_socket.close(); });
    m_context.stop();
    m_connectionState = ConnectionState::CLOSED;
}

Uploader::~Uploader() { doCloseSession(); }

void Uploader::doInitTransmission()
{
    BOOST_LOG_TRIVIAL(info) << "Init transmission";
    Filetransfer::FileTransferRequest_t request;
    request.set_file_name(m_FilePath.filename().string());
    request.set_file_size_kb(std::filesystem::file_size(m_FilePath));
    request.set_md5_hash(Utils::calculateHash(m_FilePath));

    this->doWrite(&request, &Uploader::doReadResponse);
}

void Uploader::doReadResponse()
{
    BOOST_LOG_TRIVIAL(trace) << "Wait response...";
    auto self = shared_from_this();
    doRead([self]() {
        auto rc = Utils::packageToMessage<Filetransfer::FileTransferResponse_t, decltype(self->m_data)>(self->m_data);
        switch (rc->return_code())
        {
            case Filetransfer::OK:
                BOOST_LOG_TRIVIAL(info) << "New response: OK";
                self->m_context.dispatch(std::bind(&Uploader::doTransferFile, self));
                break;
            case Filetransfer::RECEIVED:
                BOOST_LOG_TRIVIAL(info) << "New response: RECEIVED";
                self->doCloseSession();
                return;
            case Filetransfer::ERROR:
                BOOST_LOG_TRIVIAL(info) << "New response: \"ERROR " << rc->error_message() << "\"";
                self->doCloseSession();
                return;

            default:
                BOOST_LOG_TRIVIAL(error) << "Response are not defined";
        }
    });
}

void Uploader::doTransferFile()
{
    BOOST_LOG_TRIVIAL(info) << "Star transferring";

    auto self = shared_from_this();

    auto ifs  = std::ifstream(m_FilePath, std::ios::binary);
    if (!ifs)
    {
        BOOST_LOG_TRIVIAL(error) << "File didn't open";
        doCloseSession();
        return;
    }

    static std::uint32_t chunk_N{0};
    while (!ifs.eof())
    {
        auto chunk = std::make_shared<Filetransfer::FileChunk_t>();
        std::vector<char> buf(m_chunkSize);
        ifs.read(buf.data(), m_chunkSize);

        chunk->set_data(buf.data(), ifs.gcount());  // TODO potential error
        chunk->set_sequence_num(static_cast<int32_t>(chunk_N++));

        BOOST_LOG_TRIVIAL(trace) << "Send package " << chunk->sequence_num() << " " << chunk->data().size() << " byte";

        doWrite(chunk.get());  // TODO potential memory leak! (by the way, not. But better to check with Valgrind
    }

    BOOST_LOG_TRIVIAL(info) << "Send EOF flag. Chunks sent " << chunk_N;
    auto chunk = std::make_shared<Filetransfer::FileChunk_t>();
    chunk->set_sequence_num(-1);
    doWrite(chunk.get());
    self->m_context.dispatch(std::bind(&Uploader::doReadResponse, self));
}

void Uploader::doWrite(const google::protobuf::MessageLite* request, void (Uploader::*handler)())
{
    auto buf = Utils::messageToPackage(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [this, handler, buf](std::error_code ec, std::size_t) {
        if (!ec)
        {
            (this->*handler)();
        }
        else
            BOOST_LOG_TRIVIAL(error) << "Writing problem: " << asio::system_error(ec).what();
    });
}

void Uploader::doWrite(const google::protobuf::MessageLite* request)
{
    auto buf = Utils::messageToPackage(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [buf](std::error_code ec, std::size_t) {
        if (ec) BOOST_LOG_TRIVIAL(error) << "Writing problem: " << asio::system_error(ec).what();
    });
}

template <typename Handler_t>
void Uploader::doRead(Handler_t handler)
{
    auto self = shared_from_this();
    asio::async_read(
        self->m_socket, asio::buffer(m_data),
        [self](const std::error_code& ec, std::size_t received) {
            if (ec)
            {
                if (ec == asio::error::eof)
                {
                    self->doCloseSession();
                    return 0UL;
                }
                BOOST_LOG_TRIVIAL(error) << "Reading problem: " << asio::system_error(ec).what();
            }
            return Utils::receiverAlgorithm(self->m_data.begin(), received);
        },
        [self, handler](std::error_code ec, std::size_t length) {
            if (!ec)
                handler();
            else
                BOOST_LOG_TRIVIAL(error) << "Reading problem: " << asio::system_error(ec).what();
        });
}

}  // namespace Clone::TransportLayer

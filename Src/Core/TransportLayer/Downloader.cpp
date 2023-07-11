//
// Created by daniil on 6/2/23.
//

#include "Downloader.hpp"

#include <asio/streambuf.hpp>
#include <boost/log/trivial.hpp>
#include <fstream>

#include "DlManager.hpp"
#include "MessageUtils.hpp"
#include "Utils.hpp"

namespace Clone::TransportLayer
{
namespace logging = boost::log;

void Downloader::doReadNewRequest()
{
    auto self = (shared_from_this());
    doRead([self]() {
        self->m_candidate = Utils::packageToMessage<Filetransfer::FileTransferRequest_t, decltype(self->m_data)>(self->m_data);
        if (self->m_candidate)
        {
            BOOST_LOG_TRIVIAL(info) << "New candidate " << self->m_candidate->file_name() << ' ' << self->m_candidate->file_size_kb();
            BOOST_LOG_TRIVIAL(info) << "Downloading file...";

        if (request)
            self->requestValidation(request.get());
        else
        {
            BOOST_LOG_TRIVIAL(error) << "ERROR: File wasn't received correctly";
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

    doRead([self]() {
        BOOST_LOG_TRIVIAL(trace) << "Package processing";
        auto request = Utils::packageToMessage<Filetransfer::FileChunk_t, decltype(self->m_data)>(self->m_data);
        if (!request) throw std::logic_error("Damaged package was received");

        auto frame_id = request->sequence_num();
        if (frame_id == -1)
        {
            BOOST_LOG_TRIVIAL(trace) << "Package EOF granted";

            self->sendResponse(Filetransfer::RECEIVED, "RE");
            self->m_context.post(std::bind(&Downloader::assembleFile, self));
            return;
        }
        self->memory.emplace(frame_id, request->data());

        BOOST_LOG_TRIVIAL(trace) << "Got frame: " << frame_id << " " << request->data().size() << " byte";
        self->m_context.dispatch(std::bind(&Downloader::doDownload, self));
    });
}

void Downloader::assembleFile()
{
    auto self              = shared_from_this();
    auto tmpName           = Utils::genTempFileName(m_candidate->file_name());
    fs::path tmp_file_path = m_defaultFilePath.append(tmpName);  // TODO check if file already exist
    {
        std::ofstream ofs(tmp_file_path, std::ios::binary);
        for (auto&& pair : this->memory)
        {
            ofs.write(pair.second.c_str(), pair.second.size());
        }
    }
    auto hash = Utils::calculateHash(tmp_file_path);
    if (hash == m_candidate->md5_hash())  // todo when uploader send file, at the end it add extra zeros, because of that hash didn't match
    {
        BOOST_LOG_TRIVIAL(info) << "Received file hash: " << hash << " ==" << m_candidate->md5_hash();
    }
    else
        BOOST_LOG_TRIVIAL(error) << "Received file hash: " << hash << " not equal " << m_candidate->md5_hash();

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
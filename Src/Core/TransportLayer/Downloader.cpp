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

            self->sendResponse(Filetransfer::OK, "OK");
            self->m_context.post(std::bind(&Downloader::doDownload, self));
        }
        else
        {
            BOOST_LOG_TRIVIAL(error) << "ERROR: File wasn't received correctly";
            return;  // ToDo close connection??
        }
    });
}

void Downloader::doDownload()
{
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

    self->m_context.post(std::bind(&Downloader::doReadNewRequest, self));  // TODO move at true section previous if
}

template <typename Handler_t, typename... Args>
void Downloader::doWrite(const google::protobuf::MessageLite* request, Handler_t handler, Args... args)
{
    auto buf = Utils::messageToPackage(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [this, handler, args...](std::error_code ec, std::size_t) {
        if (ec) BOOST_LOG_TRIVIAL(error) << "Writing problem: " << asio::system_error(ec).what();
        if (!ec) (this->*handler)(args...);
    });
}

void Downloader::doWrite(const google::protobuf::MessageLite* request)
{
    auto buf = Utils::messageToPackage(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [](std::error_code ec, std::size_t) {
        if (ec) BOOST_LOG_TRIVIAL(error) << "Writing problem: " << asio::system_error(ec).what();
    });
}

void Downloader::doWrite(std::shared_ptr<google::protobuf::MessageLite>&& request)
{
    auto buf = Utils::messageToPackage(*request);
    asio::async_write(m_socket, asio::buffer(*buf, buf->size()), [](std::error_code ec, std::size_t) {
        if (ec) BOOST_LOG_TRIVIAL(error) << "Writing problem: " << asio::system_error(ec).what();
    });
}

template <typename Handler_t>
void Downloader::doRead(Handler_t handler)
{
    auto self = shared_from_this();
    BOOST_LOG_TRIVIAL(info) << "Wait response...";
    asio::async_read(
        self->m_socket, asio::buffer(m_data),
        [self](const std::error_code& ec, std::size_t received) {
            if (ec)
            {
                if (ec == asio::error::eof)
                {
                    self->m_dlManager.stopSession(self);
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

Downloader::Downloader(asio::io_context& ctx, tcp::socket& socket, DLManager& mngr, fs::path dfPath)
    : m_context(ctx), m_socket(std::move(socket)), m_dlManager(mngr), m_defaultFilePath(std::move(dfPath))
{
    BOOST_LOG_TRIVIAL(info) << "Connection created. Object: " << &(*this);
}

Downloader::~Downloader() { BOOST_LOG_TRIVIAL(info) << "Connection closed. Object: " << &(*this); }

void Downloader::start()
{
    m_dlManager.registerSession(shared_from_this());
    doReadNewRequest();
}

void Downloader::sendResponse(Filetransfer::ReturnCode code, std::string message)
{
    BOOST_LOG_TRIVIAL(info) << "Send response: " << code << " " << message;
    auto ok = std::make_shared<Filetransfer::FileTransferResponse_t>();
    ok->set_return_code(code);
    ok->set_error_message(message);
    this->doWrite(ok);
}

}  // namespace Clone::TransportLayer
//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_TCPACCEPTOR_HPP
#define EPAMMIDDLE_TCPACCEPTOR_HPP

#include <asio.hpp>
#include <boost/log/trivial.hpp>
#include <functional>

#include "DlManager.hpp"

namespace Clone::TransportLayer
{

namespace logging = boost::log;

using asio::ip::tcp;

class TCPAcceptor
{
    void handle_accept(const asio::error_code& error, tcp::socket peer)
    {
        BOOST_LOG_TRIVIAL(info) << "New connection!";
        if (error) BOOST_LOG_TRIVIAL(error) << "Accept error!";

        std::make_shared<Downloader>(_ioContext, peer, _manager_ptr)->start();
        start_accept();
    }

public:
    explicit TCPAcceptor(uint16_t listeningPort)
    {
        auto ep = tcp::endpoint(tcp::v4(), listeningPort);
        BOOST_LOG_TRIVIAL(info) << "Hosting up on: " << ep;

        _acceptor = std::make_unique<tcp::acceptor>(_ioContext, ep);
        start_accept();
        std::thread([this]() { _ioContext.run(); }).detach();
    }

    void start_accept()
    {
        BOOST_LOG_TRIVIAL(info) << "Listening...";
        auto error = std::placeholders::_1;
        auto peer  = std::placeholders::_2;
        _acceptor->async_accept(std::bind(&TCPAcceptor::handle_accept, this, error, peer));
    }

private:
    asio::io_context _ioContext;
    std::unique_ptr<tcp::acceptor> _acceptor;
    DLManager _manager_ptr;
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_TCPACCEPTOR_HPP

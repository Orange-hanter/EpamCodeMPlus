//
// Created by daniil on 5/19/23.
//

#ifndef EPAMMIDDLE_TCPACCEPTOR_HPP
#define EPAMMIDDLE_TCPACCEPTOR_HPP

#include <asio.hpp>
#include <functional>

#include "DlManager.hpp"

namespace Clone::TransportLayer
{

using asio::ip::tcp;

class TCPAcceptor
{
    void handle_accept(const asio::error_code& error, tcp::socket peer)
    {
        std::cout << "New connection!\n";
        // TODO: not share that extra call won't harm performance
        if (!error)
        {
            std::make_shared<Downloader>(_ioContext, peer, _manager_ptr)->start();
        }

        start_accept();
    }

public:
    explicit TCPAcceptor(uint16_t listeningPort)
    {
        auto ep = tcp::endpoint(tcp::v4(), listeningPort);
        std::cout << "Start Hosting on: " << ep << std::endl;
        _acceptor = std::make_unique<tcp::acceptor>(_ioContext, ep);
        start_accept();
        std::thread([this]() { _ioContext.run(); }).detach();
    }

    void start_accept()
    {
        std::cout << "Start listening!\n";
        auto error = std::placeholders::_1;
        auto peer  = std::placeholders::_2;
        _acceptor->async_accept(std::bind(&TCPAcceptor::handle_accept, this, error, peer));
        //[this](std::error_code ec, tcp::socket peer){
        //    this->handle_accept(ec, peer);
        //};
    }

private:
    asio::io_context _ioContext;
    std::unique_ptr<tcp::acceptor> _acceptor;
    DLManager _manager_ptr;
};

}  // namespace Clone::TransportLayer

#endif  // EPAMMIDDLE_TCPACCEPTOR_HPP

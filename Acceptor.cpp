#include "Acceptor.hpp"
#include "Connection.hpp"

#include "Utils/Net.hpp"

#include <iostream>

Acceptor::Acceptor(AEvRootConf &config, const std::string &ip, const unsigned port)
    :AEventAbstract::AEventAbstract(config),
     _acceptor(_ev_loop->get_io_service()),
     _socket(_ev_loop->get_io_service()),
     _ip(ip),
     _port(port)
{
    if (!hnet::is_ip_v4(ip))
        throw std::logic_error("Incorrect IP");
}

Acceptor::Acceptor(AEvChildConf &&config, const std::string &ip, const unsigned port)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _acceptor(_ev_loop->get_io_service()),
     _socket(_ev_loop->get_io_service()),
     _ip(ip),
     _port(port)
{
    if (!hnet::is_ip_v4(ip))
        throw std::logic_error("Incorrect IP");
}

void Acceptor::_ev_begin()
{
    asio::ip::tcp::resolver resolver(_ev_loop->get_io_service());
    asio::ip::tcp::endpoint endpoint = *resolver.resolve({_ip, std::to_string(_port)});

    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
    _start_acceept();
}

void Acceptor::_ev_finish()
{

}

void Acceptor::_ev_stop()
{

}

void Acceptor::_ev_timeout()
{

}

void Acceptor::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void Acceptor::_start_acceept()
{

    _acceptor.async_accept(_socket, _ev_loop->wrap(
                               [this] (std::error_code ec)
    {
                               if (ec) {
                                   stop();
                                   return;
                               }
                               std::cout << "new connection\n";
                               _create_child<Connection>(0, std::move(_socket));
                               _start_acceept();
                           })
            );

}


#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "AEvent/AEventAbstract.hpp"



class Acceptor : public AEventAbstract
{
public:
    Acceptor(AEvRootConf & config, const std::string &ip, const unsigned port);
    Acceptor(AEvChildConf &&config, const std::string &ip, const unsigned port);

private:
    asio::ip::tcp::acceptor _acceptor;
    asio::ip::tcp::socket _socket;
    std::string _ip;
    unsigned _port;

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _start_acceept();
};

#endif // ACCEPTOR_HPP

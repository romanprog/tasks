#include "Utils/Files.hpp"
#include "Proto.hpp"

#include <iostream>
#include <memory>
#include <string>

#include <sys/socket.h>
#include <arpa/inet.h>

class SocClient
{
public:
    SocClient()
    {}
    ~SocClient()
    {
        if (_Sock != -1)
            close(_Sock);
    }

    bool Connect(const std::string ip_, unsigned port_)
    {
        if(_Sock == -1) {
            _Sock = socket(AF_INET , SOCK_STREAM , 0);

            if (_Sock == -1)
            {
                return false;
            }
        }

        _Srv.sin_addr.s_addr = inet_addr(ip_.c_str());

        if (!_Srv.sin_addr.s_addr)
            return false;

        _Srv.sin_family = AF_INET;
        _Srv.sin_port = htons(port_);

        if (connect(_Sock , (struct sockaddr*) &_Srv , sizeof(_Srv)) < 0)
            return false;

        return true;

    }
    bool Send(const void * data_, size_t size_)
    {

        if( send(_Sock , data_ , size_ , 0) < 0)
        {
            return false;
        }

        return true;

    }

private:
    int _Sock {-1};
    std::string _Ip;
    int _Port;
    struct sockaddr_in _Srv;
    bool _Connected {false};
};

int main(int argc, char* argv[])
{
    SocClient Sender;
    if (!Sender.Connect("127.0.0.1", 8888))
        std::cout << "Connection error";

    std::string FileName {"testfile"};

    hfile::FileGuard File(FileName, hfile::FMode::ronly);
    ProtoHeader Header;
    Header.FileLenth = File.Size();
    Header.FNmaeLenth = FileName.size();

    Sender.Send(&Header, sizeof(Header));
    Sender.Send(FileName.c_str(), FileName.size());
    std::string FileDataPart;

    while (File.ReadParted(FileDataPart, 100))
        if (!Sender.Send(FileDataPart.c_str(), FileDataPart.size())) {
            std::cout << "Sending error." << std::endl;
            return 1;
        }


}

/// /////////////////////////////////////////
/// Task 2 for TrueConf.
/// TCP client.
/// Implementation:
/// Roman Progonnyj,
/// roman.progonnyj@gmail.com
/// +30 063 584 20 40
/// +38 098 545 36 68
/// /////////////////////////////////////////

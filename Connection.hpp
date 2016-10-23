#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "AEvent/AEventAbstract.hpp"
#include "Proto.hpp"
#include "Utils/Files.hpp"

class Connection: public AEventAbstract
{
public:
    explicit Connection(AEvChildConf && config, asio::ip::tcp::socket && _soc);

private:
    asio::ip::tcp::socket _socket;

    ProtoHeader _header;
    std::shared_ptr<hfile::FileGuard> _target_file;
    const size_t _data_buffer_lenth {1024};
    uint64_t _bytes_received {0};
    unsigned _prev_percent {0};

    virtual void _ev_begin() override;
    virtual void _ev_finish() override;
    virtual void _ev_stop() override;
    virtual void _ev_timeout() override;
    virtual void _ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal & _ret) override;

    void _read_header();
    void _read_filename();
    void _create_file(std::shared_ptr<std::string> fname_);
    void _read_data();
    void _write_file_part(std::shared_ptr<std::vector<char>> data_);
};

#endif // CONNECTION_HPP

#include "Connection.hpp"
#include <iostream>

Connection::Connection(AEvChildConf &&config, asio::ip::tcp::socket &&_soc)
    :AEventAbstract::AEventAbstract(std::move(config)),
     _socket(std::move(_soc))
{

}

void Connection::_ev_begin()
{
    _read_header();
}

void Connection::_ev_finish()
{

}

void Connection::_ev_stop()
{

}

void Connection::_ev_timeout()
{

}

void Connection::_ev_child_callback(AEvPtrBase child_ptr, AEvExitSignal &_ret)
{

}

void Connection::_read_header()
{
    asio::async_read(_socket,
                     asio::buffer(&_header, sizeof(ProtoHeader)),
                     _ev_loop->wrap( [this](std::error_code ec, std::size_t bytes_transferred)
                     {
                         if (ec) {
                             stop();
                             return;
                         }
                         std::cout << "Header received:" << std::endl;
                         std::cout << "Fname_sz: " << _header.FNmaeLenth << std::endl;
                         std::cout << "File_sz: " << _header.FileLenth << std::endl;

                         _read_filename();
                     }));
}

void Connection::_read_filename()
{

    if (!_header.FNmaeLenth) {
        stop();
        return;
    }

    std::shared_ptr<std::string> FNameBufferPtr = std::make_shared<std::string>();
    FNameBufferPtr->resize(_header.FNmaeLenth);

    asio::async_read(_socket,
                     asio::buffer(&(*FNameBufferPtr)[0], _header.FNmaeLenth),
                     _ev_loop->wrap( [this, FNameBufferPtr](std::error_code ec, std::size_t bytes_transferred)
                     {
                         if (ec) {
                             stop();
                             return;
                         }

                         std::cout << "Filename received:" << std::endl;
                         std::cout << "Filename: " << *FNameBufferPtr << std::endl;
                         _create_file(std::move(FNameBufferPtr));
                      }));
}

void Connection::_create_file(std::shared_ptr<std::string> fname_)
{
    unsigned FileSufix {0};

    // Create new file.
    // If file exist - add numeric sufix.
    while (hfile::exist(*fname_))
    {
        fname_->resize(_header.FNmaeLenth); // Trim old sufix.
        *fname_ += "(" + std::to_string(FileSufix) + ")";
        ++FileSufix;
    }

    _target_file = std::make_shared<hfile::FileGuard>(*fname_, hfile::FMode::wonly);
    _read_data();

}

void Connection::_read_data()
{
    std::shared_ptr<std::vector<char>> _data_buffer = std::make_shared<std::vector<char>>(_data_buffer_lenth);

    _socket.async_read_some(asio::buffer(*_data_buffer),
                            _ev_loop->wrap( [this, _data_buffer](std::error_code ec, std::size_t bytes_transferred)
                             {
                                    if (ec) {
                                        stop();
                                        return;
                                    }
                                    _data_buffer->resize(bytes_transferred);
                                    _bytes_received += bytes_transferred;
                                    _write_file_part(std::move(_data_buffer));

                              }));

}

void Connection::_write_file_part(std::shared_ptr<std::vector<char>> data_)
{
    hfile::FileAppendData WritingData(_target_file, data_);

    hfile::FileWriteQueue::Global().push_task(std::move(WritingData), [this](int err) {
        unsigned percent = static_cast<unsigned>(_bytes_received * 100.00f / _header.FileLenth);

        if ( (percent - _prev_percent > 5) || (percent == 100)) {
            std::cout << "Receiving file: " << percent << "%" << std::endl;
            _prev_percent = percent;
        }

        if (_bytes_received < _header.FileLenth) {

            _read_data();
            return;
        }
        stop();

    });

}

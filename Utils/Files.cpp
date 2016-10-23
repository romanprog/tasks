#include "Files.hpp"

#include <iostream>
#include <string.h>

namespace hfile {

bool exist(const std::string &filename)
{
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
}

FileGuard::FileGuard(const std::string &FileName_, FMode Mode_)
    :_FD( (Mode_ == FMode::ronly) ? open(FileName_.c_str(), static_cast<int>( Mode_ ))
                                  : open(FileName_.c_str(), O_CREAT | static_cast<int>( Mode_ ), 0666 )),
      _Mode (Mode_)
{
    if (_FD == -1)
        throw std::logic_error(strerror(errno));

    struct stat stat_buff{0};
    fstat(_FD, &stat_buff);
    _FileSize = stat_buff.st_size;
}

FileGuard::~FileGuard()
{
    close(_FD);
}

void FileGuard::ReadAll(std::string &FData_)
{
    if (_Mode == FMode::wonly)
        throw std::logic_error("Reading from writeonly file.");

    FData_.resize(_FileSize);
    lseek(_FD, SEEK_SET, 0);
    // read body from fd to string
    read(_FD ,&FData_[0], _FileSize);
}

size_t FileGuard::ReadParted(std::string &FData_, size_t PartSize_)
{

    if (_Mode == FMode::wonly)
        throw std::logic_error("Writing to readonly file.");

    if (!PartSize_)
        return 0;

    if (_ReadingOffset == _FileSize)
        return 0;

    FData_.resize(std::min(PartSize_, _FileSize - _ReadingOffset));
    lseek(_FD, _ReadingOffset, SEEK_SET);
    size_t readed_sz = read(_FD, &FData_[0], std::min(PartSize_, _FileSize - _ReadingOffset));
    _ReadingOffset += readed_sz;

    return readed_sz;
}

void FileGuard::ResetReadingPos()
{
    _ReadingOffset = 0;
}

FileWriteQueue & FileWriteQueue::Global()
{
    static FileWriteQueue FWriterGlobalInstance;
    return FWriterGlobalInstance;
}

void FileWriteQueue::_do_job(const FileAppendData &data_, QueueCallbackFunction &cb_)
{
    data_.File->Append(*data_.Data);
    cb_(0);
}

} // namespace hfile

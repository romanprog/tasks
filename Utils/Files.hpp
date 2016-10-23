#ifndef HFILES_HPP
#define HFILES_HPP

#include "DetachedTasksQueue/DQueueAbstract.hpp"

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <functional>

namespace hfile {

enum class FMode
{
    ronly = O_RDONLY,
    wonly = O_WRONLY,
    rdwr = O_RDWR
};

class FileGuard
{
public:
    FileGuard(const std::string & FileName_, FMode Mode_);
    ~FileGuard();
    void ReadAll(std::string & FData_);
    size_t ReadParted(std::string & FData_, size_t PartSize_);
    template <typename T>
    void Append(const T & Data_)
    {
        if (_Mode == FMode::ronly)
            throw std::logic_error("Writing to readonly file.");

        lseek(_FD, SEEK_END, -1);
        write(_FD, Data_.data(), Data_.size());

        _FileSize += Data_.size();

    }

    void ResetReadingPos();

    size_t Size()
    {
        return _FileSize;
    }

private:
    // File descriptor.
    int _FD {-1};
    size_t _FileSize {0};
    size_t _ReadingOffset {0};
    FMode _Mode;

    // Delete copy and move constructors and assignment operator.
    FileGuard(FileGuard &&) = delete;
    FileGuard & operator= (FileGuard &&) = delete;

};


struct FileAppendData
{
    FileAppendData(std::shared_ptr<FileGuard> File_, std::shared_ptr<std::vector<char>> Data_)
        : File(std::move(File_)),
          Data(std::move(Data_))
    {}

    std::shared_ptr<FileGuard> File;
    std::shared_ptr<std::vector<char>> Data;
};

using QueueCallbackFunction = std::function<void (int errnum)>;

class FileWriteQueue : public DQueueBase<FileAppendData, QueueCallbackFunction>
{
public:
    FileWriteQueue() {}

    static FileWriteQueue &Global();

private:
    virtual void _do_job(const FileAppendData & data_, QueueCallbackFunction & cb_) override;
};

bool exist(const std::string & filename);

} // namespace
#endif // HFILES_HPP

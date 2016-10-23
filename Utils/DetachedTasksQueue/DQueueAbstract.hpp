#ifndef DQUEUEABSTRACT_HPP
#define DQUEUEABSTRACT_HPP

#include <mutex>
#include <thread>
#include <queue>
#include <memory>
#include <functional>
#include <condition_variable>

//////////////////////////////////////////////////////////////////////////////////////
/// Abstract class for create detached (in own thread) queue worker. Create interface
/// for add task and stop worker loop. Contain virtual method _do_job() wich must be
/// overrided in derived class. Actual use if you want to perform a blocking action
/// as asynchronous event.
//////////////////////////////////////////////////////////////////////////////////////

// T - type of argument that describes the job.
// F - std::function obj, wich should be called from _do_job() after job finished.
template <class TaskDataType, class CallbackType>
class DQueueBase
{
    // Main queue type.
    using CallbackPair = std::pair<TaskDataType, CallbackType>;
    using AQueue = std::queue<CallbackPair>;

public:
    // Init new thread for _main_worker function with itself context (this pointer).
    DQueueBase()
        :Mthread(std::thread(&DQueueBase::_main_worker, this)) { }

    // Virtual destructor for abstract class.
    virtual ~DQueueBase() { }

    // Base method for create new job. Do not override!
    // Return immediately.
    void push_task(TaskDataType _t, CallbackType _f)
    {
        // If working loop is user stoped - dont accept new task.
        if (UserStoped)
            return;

        // Lock data accees.
        std::lock_guard<std::mutex> _lock_data(DataLock);
        // Push new job.
        MainQueue.push(std::make_pair(std::move(_t), std::move(_f)));
        // Awakene worker.
        _new_task_cond.notify_one();
        // Unlock data.
    }

    // Thread join() wrapper.
    void join()
    {
        Mthread.join();
    }

    // Causes the finalization of working loop. Bloked creation of new tasks.
    // When last task will be done - stop work and thread.
    void stop()
    {
        // Mark working loop as stoped.
        UserStoped = true;
        // Awakene worker for shutdown event.
        _new_task_cond.notify_one();
    }

protected:
    // Main jobs queue.
    AQueue MainQueue;

private:
    // Delete copy constructor.
    DQueueBase(DQueueBase &&) = delete;
    DQueueBase & operator= (DQueueBase &&) = delete;

    // Mutex for multithreading data accees.
    std::mutex DataLock;

    // Marker to finalize work.
    bool UserStoped{false};

    // Condition variable for tasks waiting.
    std::condition_variable _new_task_cond;

    // Self thread.
    std::thread Mthread;

    // Queue loop. Process mutexes and run worker of derived class.
    void _main_worker()
    {
        while (true)
        {
            std::unique_lock<std::mutex> _lock_data(DataLock);
            _new_task_cond.wait(_lock_data, [this]() {return !MainQueue.empty() || UserStoped; });

//            if (MainQueue.empty() && UserStoped)
//                // Break main loop. Stop tread.
//                return;

            // Get job info from queue.
            auto _first(std::move(MainQueue.front().first));
            auto _second(std::move(MainQueue.front().second));
            
            // Delete job.
            MainQueue.pop();

            // Unlock queue before main job.
            _lock_data.unlock();

            // Main job (in derived class).
            _do_job(_first, _second);

            if (MainQueue.empty() && UserStoped)
                // Break main loop. Stop tread.
                return;
        }
    }
    // Main virtual functtion. Get T data, do job, call F function (callback).
    // Must be owerrided in derived class
    virtual void _do_job(const TaskDataType & _t, CallbackType & _f) = 0;
};

#endif // DQUEUEABSTRACT_HPP

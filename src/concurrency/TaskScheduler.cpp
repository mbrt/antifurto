#include "TaskScheduler.hpp"

namespace antifurto {
namespace concurrency {

TaskScheduler::TaskScheduler()
    : done_(false)
    , thread_([this]{ schedulerLoop(); })
{ }

TaskScheduler::~TaskScheduler()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        done_ = true;
    }
    cv_.notify_one();
    thread_.join();
}

void TaskScheduler::scheduleAt(Clock::time_point t, Task w)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queue_.emplace(std::move(w), std::move(t));
    }
    cv_.notify_one();
}

void TaskScheduler::scheduleAfter(Clock::duration d, Task w)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queue_.emplace(std::move(w), Clock::now() + d);
    }
    cv_.notify_one();
}

void TaskScheduler::scheduleEvery(Clock::duration d, Task w)
{
    scheduleAfter(d, [=] {
        scheduleEvery(d, w);
        w();
    });
}


void TaskScheduler::schedulerLoop()
{
    Task task;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            // wait until an element is present
            while (!done_ && queue_.empty())
                cv_.wait(lock);
            if (done_) return;

            // wait until the top element time point is arrived
            auto next = queue_.top().timePoint;
            while (!done_ && Clock::now() < next)
                cv_.wait_until(lock, next);
            if (done_) return;

            task = queue_.top().task;
            queue_.pop();
        }
        task();
    }
}

} // namespace concurrency
} // namespace antifurto

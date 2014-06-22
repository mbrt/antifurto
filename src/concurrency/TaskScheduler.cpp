#include "TaskScheduler.hpp"

namespace antifurto {
namespace concurrency {

TaskScheduler::TaskScheduler()
    : done_(false)
    , semaphore_(0)
    , thread_([this]{ schedulerLoop(); })
{ }

TaskScheduler::~TaskScheduler()
{
    done_.store(true, std::memory_order_release);
    thread_.join();
}

void TaskScheduler::scheduleAt(Clock::time_point t, Task w)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    queue_.push(TaskItem{ std::move(w), std::move(t) });
    semaphore_.signal();
}

void TaskScheduler::scheduleAfter(Clock::duration d, Task w)
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    queue_.push(TaskItem{ std::move(w), Clock::now() + d });
    semaphore_.signal();
}

void TaskScheduler::scheduleEvery(Clock::duration d, Task w)
{
    scheduleAfter(std::move(d), [=] {
        scheduleEvery(d, w);
        w();
    });
}


void TaskScheduler::schedulerLoop()
{
    while (!done_.load(std::memory_order_release)) {
        workIfTaskReady();
        waitForNextTask();
    }
}

bool TaskScheduler::workIfTaskReady()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (!queue_.empty() && queue_.top().timePoint <= Clock::now()) {
        auto item = queue_.top(); queue_.pop();
        lock.unlock();  // work unlocked
        item.task();
        return true;
    }
    return false;
}

void TaskScheduler::waitForNextTask()
{
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (queue_.empty()) {
        lock.unlock();
        semaphore_.wait();
    }
    else {
        auto next = queue_.top().timePoint;
        lock.unlock();
        semaphore_.wait_until(next);
    }
}

} // namespace concurrency
} // namespace antifurto

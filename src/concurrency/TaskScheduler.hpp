#pragma once

#include <chrono>
#include <functional>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>

#include "binsem.hpp"

namespace antifurto {
namespace concurrency {

/// This class represents a scheduler of tasks.
///
/// The work is performed one after the other in one thread, so delays added by
/// one task impact on nexts. If you have long tasks, consider running them on
/// separate threads.
class TaskScheduler
{
public:
    using Task = std::function<void()>;
    using Clock = std::chrono::system_clock;

    TaskScheduler();
    ~TaskScheduler();
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator =(const TaskScheduler&) = delete;

    void scheduleAt(Clock::time_point t, Task w);
    void scheduleAfter(Clock::duration d, Task w);
    void scheduleEvery(Clock::duration d, Task w);

private:
    void schedulerLoop();
    bool workIfTaskReady();
    void waitForNextTask();

    struct TaskItem {
        Task task;
        Clock::time_point timePoint;

        TaskItem() {}
        TaskItem(Task ts, Clock::time_point tp)
            : task(std::move(ts)), timePoint(std::move(tp)) {}
    };
    struct TaskComparator {
        bool operator()(const TaskItem& a, const TaskItem& b) const {
            return a.timePoint < b.timePoint;
        }
    };

    using Queue = std::priority_queue<TaskItem,
        std::vector<TaskItem>,
        TaskComparator>;

    Queue queue_;
    std::mutex queueMutex_;
    std::atomic<bool> done_;
    binsem semaphore_;
    std::thread thread_;
};

} // namespace concurrency
} // namespace antifurto

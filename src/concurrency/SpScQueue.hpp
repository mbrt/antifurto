#pragma once

#include <atomic>
#include <thread>
#include <iostream>

#if defined(USE_BOOST_LOCKFREE)
#   include <boost/lockfree/spsc_queue.hpp>
#else
#   include <folly/ProducerConsumerQueue.hpp>
#endif

#include <mutex>
#include <condition_variable>

namespace antifurto {
namespace concurrency {


template <typename T, typename F>
class SpScQueue
{
public:
    SpScQueue(F workFunction = F(), std::size_t queueSize = 1024)
        : working_(ATOMIC_FLAG_INIT), done_(false)
        , queue_(queueSize), workFunc_(workFunction)
        , worker_([this]{ work(); })
    { }

    ~SpScQueue() {
        // exit even if the work has not been completed
        done_ = true;
        sem_.notify_one();
        worker_.join();
    }

    template <typename U>
    bool enqueue(U&& value) {
        bool enqueued = queue_.write(std::forward<U>(value));
        if (!working_.test_and_set())
            sem_.notify_one();
        return enqueued;
    }

private:
    void work() {
        T value;
        while (!done_) {
            waitNotify();
            while (!done_ && queue_.read(value))
                workFunc_(value);
            working_.clear();
            while (!done_ && queue_.read(value))
                workFunc_(value);
        }
    }

    inline void waitNotify()
    {
        std::unique_lock<std::mutex> lock(m_);
        sem_.wait(lock);
        // spurious wake-ups are not problematic
    }

    using Queue = folly::ProducerConsumerQueue<T>;

    std::atomic_flag working_;
    std::atomic<bool> done_;
    Queue queue_;
    F workFunc_;
    std::mutex m_;
    std::condition_variable sem_;
    std::thread worker_;
};

} // namespace concurrency
} // namespace antifurto

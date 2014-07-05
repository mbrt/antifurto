#pragma once

#include <atomic>
#include <thread>
#include <iostream>

#if defined(USE_BOOST_LOCKFREE)
#   include <boost/lockfree/spsc_queue.hpp>
#else
#   include <folly/ProducerConsumerQueue.hpp>
#endif

#include "binsem.hpp"

namespace antifurto {
namespace concurrency {


template <typename T, typename F>
class SpScQueue
{
public:
    SpScQueue(F workFunction = F(), std::size_t queueSize = 1024)
        : working_(ATOMIC_FLAG_INIT), done_(false), semaphore_(0)
        , queue_(queueSize)
        , workFunc_(std::move(workFunction))
        , worker_([this]{ work(); })
    { }

    ~SpScQueue() {
        // exit even if the work has not been completed
        done_ = true;
        semaphore_.signal();
        worker_.join();
    }

    template <typename U>
    bool enqueue(U&& value) {
        bool enqueued = queue_.write(std::forward<U>(value));
        if (!working_.test_and_set())
            semaphore_.signal();
        return enqueued;
    }

private:
    void work() {
        T value;
        while (!done_) {
            semaphore_.wait();
            while (!done_ && queue_.read(value))
                workFunc_(value);
            working_.clear();
            while (!done_ && queue_.read(value))
                workFunc_(value);
        }
    }

    using Queue = folly::ProducerConsumerQueue<T>;

    std::atomic_flag working_;
    std::atomic<bool> done_;
    binsem semaphore_;
    Queue queue_;
    F workFunc_;
    std::thread worker_;
};

} // namespace concurrency
} // namespace antifurto

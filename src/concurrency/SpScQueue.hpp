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


#if defined(USE_BOOST_LOCKFREE)
template <typename T, typename F, int Capacity = 1024>
#else
template <typename T, typename F>
#endif
class SpScQueue
{
public:
#if defined(USE_BOOST_LOCKFREE)
    SpScQueue(F workFunction = F())
        : working_(ATOMIC_FLAG_INIT), done_(false), semaphore_(0)
        , workFunc_(std::move(workFunction))
        , worker_([this]{ work(); })
#else
    SpScQueue(F workFunction = F(), std::size_t queueSize = 1024)
        : working_(ATOMIC_FLAG_INIT), done_(false), semaphore_(0)
        , queue_(queueSize)
        , workFunc_(std::move(workFunction))
        , worker_([this]{ work(); })
#endif
    { }

    ~SpScQueue() {
        // exit even if the work has not been completed
        done_ = true;
        semaphore_.signal();
        worker_.join();
    }

    template <typename U>
    bool enqueue(U&& value) {
        bool enqueued = queueWrite(std::forward<U>(value));
        if (!working_.test_and_set())
            semaphore_.signal();
        return enqueued;
    }

private:
    void work() {
        T value;
        while (!done_) {
            semaphore_.wait();
            while (!done_ && queueRead(value))
                workFunc_(value);
            working_.clear();
            while (!done_ && queueRead(value))
                workFunc_(value);
        }
    }

    inline bool queueRead(T& value)
    {
#if defined(USE_BOOST_LOCKFREE)
        return queue_.pop(value);
#else
        return queue_.read(value);
#endif
    }

    template <typename U>
    inline bool queueWrite(U&& value)
    {
#if defined(USE_BOOST_LOCKFREE)
        return queue_.push(std::forward<U>(value));
#else
        return queue_.write(std::forward<U>(value));
#endif
    }

#if defined(USE_BOOST_LOCKFREE)
    using Queue = boost::lockfree::spsc_queue<
                    T,
                    boost::lockfree::capacity<Capacity>>;
#else
    using Queue = folly::ProducerConsumerQueue<T>;
#endif

    std::atomic_flag working_;
    std::atomic<bool> done_;
    binsem semaphore_;
    Queue queue_;
    F workFunc_;
    std::thread worker_;
};

} // namespace concurrency
} // namespace antifurto

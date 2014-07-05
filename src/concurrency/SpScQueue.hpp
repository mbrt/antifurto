#pragma once

#include <atomic>
#include <thread>
#include <iostream>

#if defined(SPSCQ_USE_BOOST_LOCKFREE)
#   include <boost/lockfree/spsc_queue.hpp>
#else
#   include <folly/ProducerConsumerQueue.hpp>
#endif

// #define SPSCQ_NO_BRANCH_OPT

#if defined(__GNUC__) && !defined(SPSCQ_NO_BRANCH_OPT)
#define SPSCQ_LIKELY(x)   __builtin_expect((x),1)
#define SPSCQ_UNLIKELY(x) __builtin_expect((x),0)
#else
#define SPSCQ_LIKELY(x)   x
#define SPSCQ_UNLIKELY(x) x
#endif

#include "binsem.hpp"

namespace antifurto {
namespace concurrency {

constexpr int defaultCapacity() { return 1024; }

/// Lockfree single producer single consumer queue.
///
/// This queue is different from the standard implementation, because it
/// does not use busy wait. When the queue is empty, the worker thread stops
/// in a semaphore.
/// Purpose of this queue is to make enqueue extremely fast
#if defined(SPSCQ_USE_BOOST_LOCKFREE)
template <typename T, typename F, int Capacity = defaultCapacity()>
#else
template <typename T, typename F>
#endif
class SpScQueue
{
public:
#if defined(SPSCQ_USE_BOOST_LOCKFREE)
    SpScQueue(F workFunction = F())
        : working_(ATOMIC_FLAG_INIT), done_(false), semaphore_(0)
        , workFunc_(std::move(workFunction))
        , worker_([this]{ work(); })
#else
    SpScQueue(F workFunction = F(), std::size_t queueSize = defaultCapacity())
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
        if (SPSCQ_UNLIKELY(!working_.test_and_set()))
            semaphore_.signal();
        return enqueued;
    }

private:
    void work() {
        T value;
        while (SPSCQ_LIKELY(!done_)) {
            semaphore_.wait();
            while (SPSCQ_LIKELY(!done_ && queueRead(value)))
                workFunc_(value);
            working_.clear();
            while (SPSCQ_UNLIKELY(!done_ && queueRead(value)))
                workFunc_(value);
        }
    }

#if defined(SPSCQ_USE_BOOST_LOCKFREE)
    inline bool queueRead(T& value)
    {
        return queue_.pop(value);
    }

    template <typename U>
    inline bool queueWrite(U&& value)
    {
        return queue_.push(std::forward<U>(value));
    }

    using Queue = boost::lockfree::spsc_queue<
                    T,
                    boost::lockfree::capacity<Capacity>>;
#else
    inline bool queueRead(T& value)
    {
        return queue_.read(value);
    }

    template <typename U>
    inline bool queueWrite(U&& value)
    {
        return queue_.write(std::forward<U>(value));
    }

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

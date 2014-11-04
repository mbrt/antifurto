#pragma once

#include <atomic>
#include <mutex>
#include <functional>

namespace antifurto {
namespace meta {

template <typename T>
class Singleton
{
public:
    /// this function builds the actual singleton object
    /// without the need for synchronization operations
    using Factory = std::function<T*()>;

    Singleton(Factory factory)
        : factory_(std::move(factory))
        , instance_(nullptr)
    { }

    T& instance()
    {
        T* tmp = instance_.load(std::memory_order_consume);
        if (!tmp) {
            std::lock_guard<std::mutex> guard{instance_mutex};
            tmp = instance_.load(std::memory_order_consume);
            if (!tmp) {
                tmp = factory_();
                instance_.store(tmp, std::memory_order_release);
            }
        }
        return *tmp;
   }

private:
    Factory factory_;
    std::atomic<T*> instance_;
    std::mutex instance_mutex;
};

}} // namespace antifurto::meta

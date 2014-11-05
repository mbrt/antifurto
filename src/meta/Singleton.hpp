#pragma once

#include <atomic>
#include <mutex>
#include <functional>

namespace antifurto {
namespace meta {

/// This class implements a singleton that holds generic objects.
///
/// @tparam T the type that have to be held in the singleton.
/// @tparam function type that builds the actual singleton object
///         without the need for synchronization operations.
template <typename T, typename F = std::function<T*()>>
class Singleton
{
public:
    Singleton(F factory)
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
    F factory_;
    std::atomic<T*> instance_;
    std::mutex instance_mutex;
};

}} // namespace antifurto::meta

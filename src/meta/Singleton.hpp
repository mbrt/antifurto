#pragma once

#include <atomic>
#include <mutex>
#include <functional>

namespace antifurto {
namespace meta {
namespace detail {

/// This class holds generic objects, building them in a lazy way.
///
/// @tparam T the type that have to be held in the singleton.
/// @tparam F function type that builds the actual singleton object
///         without the need for synchronization operations.
template <typename T, typename F = std::function<T*()>>
class Singleton
{
public:
    Singleton(F factory = []{ return new T;})
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

} // namespace detail


/// A generic singleton class
/// @tparam T the type to hold.
template <typename T>
class Singleton
{
public:
    Singleton() = delete;
    Singleton(const Singleton& ) = delete;
    Singleton& operator =(const Singleton& ) = delete;

    static T& instance() {
        return impl_.instance();
    }

private:
    static detail::Singleton<T> impl_;
};


template <typename T>
detail::Singleton<T> Singleton<T>::impl_{};

}} // namespace antifurto::meta

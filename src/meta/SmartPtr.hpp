#pragma once

#include <memory>
#include <functional>

namespace antifurto {
namespace meta {

namespace detail {

template <typename T>
struct ErasedDeleter : std::function<void(T*)>
{
    ErasedDeleter()
        : std::function<void(T*)> { [](T * p){delete p;} } {}
};

} // namespace detail


template <typename T>
using ErasedUniquePtr = std::unique_ptr<T, detail::ErasedDeleter<T>>;

} // namespace meta
} // namespace antifurto

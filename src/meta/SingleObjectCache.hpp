#pragma once

#include <memory>
#include <mutex>

namespace antifurto {
namespace meta {

/// This class caches an object.
/// The object is shared among clients; whenever all clients drops it,
/// it is automatically cleaned up. It is is re-created when a new
/// client comes.
template <typename T>
class SingleObjectCache
{
public:
    using Ptr = std::shared_ptr<T>;

    Ptr get()
    {
        Ptr ctx = context_.lock();
        if (ctx) return ctx;

        std::lock_guard<std::mutex> lock{m_};
        // double checked locking pattern
        if (ctx = context_.lock()) return ctx;
        context_ = ctx = std::make_shared<T>();
        return ctx;
    }

private:
    using WeakPtr = std::weak_ptr<T>;

    WeakPtr context_;
    std::mutex m_;
};

}} // namespace antifurto::meta

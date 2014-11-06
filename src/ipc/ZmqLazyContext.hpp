#pragma once

#include <memory>
#include <mutex>
#include <zmq.hpp>

#include "../meta/Singleton.hpp"

namespace antifurto {
namespace ipc {
namespace detail {

/// This class caches a ZeroMQ context.
/// The context is shared among clients; whenever all clients drops it,
/// it is automatically cleaned up. The context is re-created when a new
/// client comes.
class ZmqLazyContext
{
public:
    using Ptr = std::shared_ptr<zmq::context_t>;

    Ptr getContext();

private:
    using WeakPtr = std::weak_ptr<zmq::context_t>;

    WeakPtr context_;
    std::mutex m_;
};

} // namespace detail


/// This is the singleton holding the ZeroMQ context.
using ZmqLazyContext = meta::Singleton<detail::ZmqLazyContext>;

}} // namespace antifurto::ipc

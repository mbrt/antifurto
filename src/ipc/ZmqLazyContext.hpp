#pragma once

#include <memory>
#include <mutex>
#include <zmq.hpp>

#include "../meta/Singleton.hpp"
#include "../meta/SingleObjectCache.hpp"

namespace antifurto {
namespace ipc {
namespace detail {

using ZmqLazyContext = meta::SingleObjectCache<zmq::context_t>;

} // namespace detail


/// This is the singleton holding the ZeroMQ context.
/// The context is shared among clients; whenever all clients drops it,
/// it is automatically cleaned up. The context is re-created when a new
/// client comes.
///
/// @example
/// @code
/// ZmqContextPtr ctxPtr = ZmqLazyContext::instance().get();
/// zmq::context_t& ctx = *ctxPtr;
/// @endcode
using ZmqLazyContext = meta::Singleton<detail::ZmqLazyContext>;

/// This is a shared pointer containing a ZeroMQ context.
using ZmqContextPtr = detail::ZmqLazyContext::Ptr;

}} // namespace antifurto::ipc

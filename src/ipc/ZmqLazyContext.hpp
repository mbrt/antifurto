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
using ZmqLazyContext = meta::Singleton<detail::ZmqLazyContext>;

}} // namespace antifurto::ipc

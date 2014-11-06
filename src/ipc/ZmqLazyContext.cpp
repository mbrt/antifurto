#include "ZmqLazyContext.hpp"

namespace antifurto {
namespace ipc {
namespace detail {

ZmqLazyContext::Ptr ZmqLazyContext::getContext()
{
    Ptr ctx = context_.lock();
    if (ctx) return ctx;

    std::lock_guard<std::mutex> lock{m_};
    // double checked locking pattern
    if (ctx = context_.lock()) return ctx;
    context_ = ctx = std::make_shared<zmq::context_t>();
    return ctx;
}

}}} // namespace antifurto::ipc::detail

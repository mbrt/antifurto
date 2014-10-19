#include "ZmqRepServer.hpp"

namespace antifurto {
namespace ipc {

ZmqRepServer::ZmqRepServer(zmq::context_t& ctx, std::string address)
    : context_(ctx), address_(std::move(address))
    , started_(false)
{ }

void ZmqRepServer::start()
{
    bool expected = false;
    if (started_.compare_exchange_strong(expected, true)) {
        socket_.reset(new zmq::socket_t{context_, ZMQ_REP});
        socket_->connect(address_.c_str());
    }
}

void ZmqRepServer::stop()
{

}

}} // namespace antifurto::ipc

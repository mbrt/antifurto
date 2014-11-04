#include "ZmqRepServer.hpp"

namespace antifurto {
namespace ipc {

ZmqRepServer::ZmqRepServer(zmq::context_t& ctx, std::string address, Handler h)
    : context_(ctx), address_(std::move(address)), handler_(std::move(h))
    , running_(false)
{ }

ZmqRepServer::~ZmqRepServer()
{
    stop();
}

void ZmqRepServer::start()
{
    std::unique_lock<std::mutex> lock{startMutex_};
    if (!running_) {
        socket_.reset(new zmq::socket_t{context_, ZMQ_REP});
        socket_->connect(address_.c_str());
        const int timeout = 2500;
        socket_->setsockopt(ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
        socket_->setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
        thread_ = std::thread(&ZmqRepServer::receiveLoop, this);
        running_ = true;
    }
}

void ZmqRepServer::stop()
{
    {
        std::unique_lock<std::mutex> lock{startMutex_};
        if (!running_) return;

        running_ = false;
        socket_->close();
    }
    thread_.join();
}

bool ZmqRepServer::started()
{
    return running_;
}


void ZmqRepServer::receiveLoop()
{
    while (running_) {
        while (!socket_->recv(&request_) && running_) { }
        if (!running_) return;
        // message received
        zmq::message_t& reply = handler_(request_);
        // send the reply
        while (!socket_->send(reply) && running_) { }
    }
}

}} // namespace antifurto::ipc

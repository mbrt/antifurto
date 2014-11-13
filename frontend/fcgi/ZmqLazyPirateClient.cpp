#include "ZmqLazyPirateClient.hpp"

namespace antifurto {
namespace fcgi {

ZmqLazyPirateClient::
ZmqLazyPirateClient(zmq::context_t& ctx, std::string serverAddress,
                    unsigned int timeout, unsigned int retries)
    : context_(ctx), address_(std::move(serverAddress))
    , timeout_(timeout), retries_(retries)
{
    openSocket();
}

bool ZmqLazyPirateClient::
request(zmq::message_t& request, zmq::message_t& reply)
{
    unsigned int retriesLeft = retries_;
    while (retriesLeft-- > 0) {
        socket_->send(request);
        //  Poll socket for a reply, with timeout
        zmq::pollitem_t items[] = { { *socket_, 0, ZMQ_POLLIN, 0 } };
        zmq::poll(&items[0], 1, timeout_);
        if (items[0].revents & ZMQ_POLLIN) {
            if (socket_->recv(&reply))
                return true;
        }
        openSocket();
    }
    return false;
}

void ZmqLazyPirateClient::openSocket()
{
    socket_.reset(new zmq::socket_t{context_, ZMQ_REQ});
    socket_->connect(address_.c_str());
    int linger = 0;
    socket_->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
}

}} // namespace antifurto::fcgi

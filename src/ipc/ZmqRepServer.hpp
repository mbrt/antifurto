#pragma once

#include <zmq.hpp>
#include <atomic>
#include <memory>

namespace antifurto {
namespace ipc {

/// This class implements a ZeroMQ REP server.
class ZmqRepServer
{
public:
    ZmqRepServer(zmq::context_t& ctx, std::string address);

    void start();
    void stop();
    bool started();

    bool receive(zmq::message_t& msg);
    bool send(const zmq::message_t& msg);

private:
    zmq::context_t& context_;
    std::string address_;
    std::atomic<bool> started_;
    std::unique_ptr<zmq::socket_t> socket_;
};

}} // namespace antifurto::ipc

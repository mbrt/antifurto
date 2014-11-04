#pragma once

#include <zmq.hpp>
#include <mutex>
#include <memory>
#include <functional>
#include <thread>

namespace antifurto {
namespace ipc {

/// This class implements a ZeroMQ REP server.
class ZmqRepServer
{
public:
    /// The handler gets the request message and have to return
    /// the reply
    using Handler = std::function<zmq::message_t&(const zmq::message_t&)>;

    ZmqRepServer(zmq::context_t& ctx, std::string address, Handler h);
    ~ZmqRepServer();

    void start();
    void stop();
    bool started();

private:
    void receiveLoop();

    zmq::context_t& context_;
    std::string address_;
    Handler handler_;
    bool running_;
    std::mutex startMutex_;
    std::unique_ptr<zmq::socket_t> socket_;
    std::thread thread_;
    zmq::message_t request_;
};

}} // namespace antifurto::ipc

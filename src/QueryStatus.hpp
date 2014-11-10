#pragma once

#include "ipc/ZmqRepServer.hpp"
#include "ipc/ZmqLazyContext.hpp"

namespace antifurto {

// fwd references
class Antifurto;
class Configuration;

/// This class is responsible of listening to a port with a zmq socket to
/// possible requests for the global status of the application
class AntifurtoQuery
{
public:
    AntifurtoQuery(Antifurto& controller, Configuration& config);
    void start();
    void stop();

private:
    zmq::message_t& handler(const zmq::message_t& req);

    Antifurto& controller_;
    zmq::message_t reply_;
    ipc::ZmqContextPtr zmqCtx_;
    ipc::ZmqRepServer server_;
};

} // namespace antifurto

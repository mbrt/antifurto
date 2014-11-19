#pragma once

#include "ipc/ZmqRepServer.hpp"
#include "ipc/ZmqLazyContext.hpp"
#include "../serialization/DefaultZmqSerializer.hpp"

namespace antifurto {

// fwd references
class Antifurto;
class Configuration;

/// This class is responsible of listening to a port with a zmq socket to
/// possible requests for the global status of the application
class AntifurtoQuery
{
public:
    AntifurtoQuery(Antifurto& controller, const Configuration& config);
    void start();
    void stop();

private:
    zmq::message_t& handler(const zmq::message_t& req);
    void handleMonitorStatusRequest();
    void handleUnknownRequest(serialization::MessageType type);

    Antifurto& controller_;
    zmq::message_t reply_;
    ipc::ZmqContextPtr zmqCtx_;
    serialization::DefaultZmqSerializer serializer_;
    ipc::ZmqRepServer server_;
};

} // namespace antifurto

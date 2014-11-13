#include "QueryStatus.hpp"
#include "Config.hpp"
#include "Antifurto.hpp"
#include "log/Log.hpp"

namespace antifurto {

AntifurtoQuery::
AntifurtoQuery(Antifurto& controller, const Configuration& config)
    : controller_(controller)
    , zmqCtx_(ipc::ZmqLazyContext::instance().get())
    , server_(*zmqCtx_, "tcp://" + config.query.socketAddress,
              [this](const zmq::message_t& m) -> zmq::message_t& {
                  return handler(m);
              })
{ }

void AntifurtoQuery::start()
{
    server_.start();
}

void AntifurtoQuery::stop()
{
    server_.stop();
}

zmq::message_t&AntifurtoQuery::handler(const zmq::message_t& req)
{
    using namespace serialization;

    MessageType reqT = serializer_.deserializeHeader(req);
    switch (reqT) {
    case MessageType::MONITOR_STATUS_REQUEST:
        handleMonitorStatusRequest();
        break;
    default:
        handleUnknownRequest(reqT);
    }

    return reply_;
}

void AntifurtoQuery::handleMonitorStatusRequest()
{
    using namespace serialization;

    auto status = static_cast<ServiceStatus>(controller_.getMonitoringStatus());
    MonitorStatusReply reply{status};
    serializer_.serializeMessage(reply_,
                                 MessageType::MONITOR_STATUS_REPLY,
                                 reply);
}

void AntifurtoQuery::handleUnknownRequest(serialization::MessageType type)
{
    using namespace serialization;
    log::warning() << "Unknown request received: " << static_cast<int>(type);
    serializer_.serializeMessage(reply_, MessageType::ERROR_UNKNOWN_MESSAGE);
}

} // namespace antifurto

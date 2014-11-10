#include "QueryStatus.hpp"
#include "Config.hpp"

namespace antifurto {

AntifurtoQuery::
AntifurtoQuery(Antifurto& controller, Configuration& config)
    : controller_(controller)
    , zmqCtx_(ipc::ZmqLazyContext::instance().get())
    , server_(*zmqCtx_, config.query.socketAddress,
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

zmq::message_t&AntifurtoQuery::handler(const zmq::message_t&)
{
    return reply_;
}

} // namespace antifurto

#pragma once

#include <zmq.hpp>
#include <string>
#include <memory>

namespace antifurto {
namespace fcgi {

/// This class implements the lazy pirate pattern in zeromq.
/// See: http://zguide.zeromq.org/page:all#Client-Side-Reliability-Lazy-Pirate-Pattern
class ZmqLazyPirateClient
{
public:
    ZmqLazyPirateClient(zmq::context_t& ctx,
                        std::string serverAddress,
                        unsigned int timeout = 10000,
                        unsigned int retries = 3);
    
    /// Performs a request-reply transition.
    /// @param request the request message to send to the server
    /// @param reply the message in which store the server reply
    /// @return true if a reply arrived within the timeout
    bool request(zmq::message_t& request, zmq::message_t& reply);
    
private:
    void openSocket();

    zmq::context_t& context_;
    std::string address_;
    unsigned int timeout_;
    unsigned int retries_;
    std::unique_ptr<zmq::socket_t> socket_;
    zmq::message_t requestBackup_;
};

}} // namespace antifurto::fcgi

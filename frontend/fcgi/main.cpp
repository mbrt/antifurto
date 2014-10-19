// see example here:
// https://github.com/toshic/libfcgi/blob/master/examples/echo-cpp.cpp

#include <fcgio.h>
#include <fcgi_config.h>
#include <iostream>
#include <string>

#include "StreamRedirect.hpp"
#include "StreamReader.hpp"
#include "ZmqLazyPirateClient.hpp"
#include "serialization/DefaultZmqSerializer.hpp"
#include "serialization/Protocol.hpp"

using namespace antifurto::fcgi;
using namespace antifurto::serialization;

constexpr const char* serverAddress() {
    return "tcp://localhost:4679";
}

void beginResponse(const char* status)
{
    std::cout << "Status: " << status << "\r\n\r\n";
}

void beginResponse(const char* status, const char* contentType, bool cache = false)
{
    std::cout << "Status: " << status << "\r\n"
                 "Content-Type: " << contentType << "\r\n";
    if (!cache)
        std::cout << "Cache-Control: no-cache\r\n";
    std::cout << "\r\n";
}

/// This class handle communication with main exe and send responses back
/// to the fcgi daemon
class RequestHandler
{
public:
    RequestHandler()
        : zmqctx_(1)
        , client_(zmqctx_, serverAddress(), 2500, 3)
    {
        serializer_.serializeMessage(liveRequestMsg_,
                                     MessageType::LIVE_VIEW_REQUEST);
    }

    void handleRequest(const char* path)
    {
        if (path == nullptr)
            beginResponse("417 Expectation Failed");
        else if (::strcmp(path, "/live/live.jpg") == 0)
            handleLiveViewRequest();
        else
            beginResponse("501 Not Implemented");
    }

private:
    void handleLiveViewRequest()
    {
        // talk to main exe if found
        if (client_.request(liveRequestMsg_, replyMsg_)) {
            // got answer, send image
            beginResponse("200 OK", "image/jpeg", false);
            std::cout.write(
                static_cast<const char*>(replyMsg_.data()),
                replyMsg_.size());
        }
        else
            // no answer, send service unavailable
            beginResponse("503 Service Not Available");
    }

    zmq::context_t zmqctx_;
    ZmqLazyPirateClient client_;
    DefaultZmqSerializer serializer_;
    zmq::message_t liveRequestMsg_;
    zmq::message_t replyMsg_;
};


int main(int, char*[])
{
    RequestHandler handler;
    StreamReader streamReader{std::cin, 10 * 1024};

    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        // redirect stdio to the request streams
        StreamRedirector redirect{request};
        // ignore inputs
        streamReader.emptyStream();
        // handle the request
        handler.handleRequest(FCGX_GetParam("SCRIPT_NAME", request.envp));
    }
    return 0;
}

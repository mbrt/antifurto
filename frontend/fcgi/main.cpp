// see example here:
// https://github.com/toshic/libfcgi/blob/master/examples/echo-cpp.cpp

#include <fcgio.h>
#include <fcgi_config.h>
#include <iostream>

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

int main(int, char*[])
{
    zmq::context_t zmqctx(1);
    ZmqLazyPirateClient client{zmqctx, serverAddress(), 2500, 3};
    DefaultZmqSerializer serializer;
    zmq::message_t liveRequestMsg, replyMsg;
    serializer.serializeMessage(liveRequestMsg, MessageType::LIVE_VIEW_REQUEST);

    StreamReader streamReader{std::cin, 10 * 1024};

    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        // redirect stdio to the request streams
        StreamRedirector redirect{request};
        // ignore inputs
        streamReader.emptyStream();
        // talk to main exe if found
        if (client.request(liveRequestMsg, replyMsg)) {
            // got answer, send image
            beginResponse("200 OK", "image/jpeg", false);
            std::cout.write(
                static_cast<const char*>(replyMsg.data()),
                replyMsg.size());
        }
        else {
            // no answer, send service unavailable
            beginResponse("503 Service Not Available");
        }
    }
    return 0;
}

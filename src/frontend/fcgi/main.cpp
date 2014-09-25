// see example here:
// https://github.com/toshic/libfcgi/blob/master/examples/echo-cpp.cpp

#include <fcgio.h>
#include <fcgi_config.h>
#include <iostream>
#include <b64/encode.h>

#include "StreamRedirect.hpp"
#include "StreamReader.hpp"
#include "ZmqLazyPirateClient.hpp"

constexpr const char* serverAddress() {
    return "tcp://localhost:4679";
}

zmq::message_t makeRequest()
{
    int magic = 158;
    zmq::message_t result{sizeof(magic)};
    ::memcpy(result.data(), &magic, sizeof(magic));
    return result;
}

using namespace antifurto::fcgi;

int main(int, char*[])
{
    zmq::context_t zmqctx(1);
    ZmqLazyPirateClient client{zmqctx, serverAddress(), 2500, 3};
    zmq::message_t requestMsg = makeRequest();
    zmq::message_t replyMsg;

    StreamReader streamReader{std::cin, 10 * 1024};
    base64::encoder b64encoder{4096};

    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        // redirect stdio to the request streams
        StreamRedirector redirect{request};
        // ignore inputs
        streamReader.emptyStream();
        // talk to main exe if found
        if (client.request(requestMsg, replyMsg)) {
            // got answer, send image
            std::cout << "Content-Type: image/jpeg\r\n"
                "Status: 200 OK\r\n"
                "Cache-Control: no-cache\r\n"
                "\r\n";
            b64encoder.encode(
                static_cast<const char*>(replyMsg.data()),
                replyMsg.size(), std::cout);
        }
        else {
            // no answer, send service unavailable
            std::cout << "Content-Type: text/html\r\n"
                "Status: 503 Service Not Available\r\n";
        }
    }
    return 0;
}

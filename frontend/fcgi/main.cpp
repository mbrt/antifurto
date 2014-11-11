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

constexpr const char* liveServerAddress() {
    return "tcp://localhost:4679";
}

constexpr const char* queryServerAddress() {
    return "tcp://localhost:4678";
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
        , liveClient_(zmqctx_, liveServerAddress(), 2500, 3)
        , queryClient_(zmqctx_, queryServerAddress(), 2500, 3)
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
        else if (::strcmp(path, "/controller/controller?query=monitor-status") == 0)
            handleQueryRequest();
        else
            beginResponse("501 Not Implemented");
    }

private:
    void handleLiveViewRequest()
    {
        // talk to main exe if found
        if (liveClient_.request(liveRequestMsg_, replyMsg_)) {
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

    void handleQueryRequest()
    {
        // talk to main exe if found
        if (queryClient_.request(queryRequestMsg_, replyMsg_)) {
            MessageType header = serializer_.deserializeHeader(replyMsg_);
            if (header == MessageType::MONITOR_STATUS_REPLY) {
                MonitorStatusReply contents =
                    serializer_.deserializePayload<MonitorStatusReply>(replyMsg_);

                bool active, in_progress;
                switch (contents.status) {
                case ServiceStatus::RUNNING:
                    {
                        active = true;
                        in_progress = false;
                    }
                    break;
                case ServiceStatus::STARTING:
                    {
                        active = true;
                        in_progress = true;
                    }
                    break;
                case ServiceStatus::STOPPED:
                    {
                        active = false;
                        in_progress = false;
                    }
                    break;
                case ServiceStatus::STOPPING:
                    {
                        active = false;
                        in_progress = true;
                    }
                    break;
                default:
                    {
                        beginResponse("503 Service Not Available");
                        return;
                    }
                }

                beginResponse("200 OK", "application/json", false);
                std::cout << "{ active: " << (active ? "true" : "false")
                          << ", in_progress: " << (in_progress ? "true" : "false")
                          << " }";
            }
            else
                beginResponse("500 Internal Server Error");
        }
        else
            // no answer, send service unavailable
            beginResponse("503 Service Not Available");
    }

    zmq::context_t zmqctx_;
    ZmqLazyPirateClient liveClient_;
    ZmqLazyPirateClient queryClient_;
    DefaultZmqSerializer serializer_;
    zmq::message_t liveRequestMsg_;
    zmq::message_t queryRequestMsg_;
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

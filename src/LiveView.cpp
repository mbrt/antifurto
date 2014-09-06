#include "LiveView.hpp"
#include "Log.hpp"
#include "text/ToString.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;


namespace antifurto {

LiveView::LiveView(const std::string& socketAddress)
    : context_(1)
    , socket_(context_, ZMQ_REP)
    , running_(true)
    , worker_([&](const Picture& p){ write(p); }, 3)
{
    int timeout = 2500;
    socket_.setsockopt(ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
    socket_.setsockopt(ZMQ_RCVTIMEO, &timeout, sizeof(timeout));
    try {
        socket_.bind(("tcp://" + socketAddress).c_str());
    }
    catch (std::exception& e) {
        LOG_ERROR << "Error binding live view socket: " << e.what();
        throw;
    }
}

LiveView::~LiveView()
{
    running_.store(false, std::memory_order_release);
}

bool LiveView::addPicture(const Picture& p)
{
    return worker_.enqueue(p);
}

void LiveView::write(const Picture& p)
{
    if (!running_.load(std::memory_order_acquire)) return;

    try {
        // first of all encode
        if (!cv::imencode(".jpg", p, imageBuffer_, {CV_IMWRITE_JPEG_QUALITY, 75}))
            throw Exception("Cannot save image to buffer in live view");

        // now wait the request
        zmq::message_t request;
        while (!socket_.recv(&request) && running_.load(std::memory_order_acquire)) { }
        if (!running_.load(std::memory_order_acquire)) return;

        // reply with the contents
        zmq::message_t reply{imageBuffer_.size()};
        ::memcpy((void*)reply.data(), &imageBuffer_[0], imageBuffer_.size());
        while (!socket_.send(reply) && running_.load(std::memory_order_acquire)) { }
    }
    catch (std::exception& e) {
        LOG_ERROR << "Error saving image for live view: " << e.what();
    }
}

} // namespace antifurto

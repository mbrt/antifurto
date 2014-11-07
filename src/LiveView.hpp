#pragma once

#include "Picture.hpp"
#include "Exception.hpp"
#include "concurrency/SpScQueue.hpp"

#include <string>
#include <vector>
#include <atomic>
#include <zmq.hpp>

namespace antifurto {

/// This is the controller for the live view
class LiveView
{
public:
    LiveView(zmq::context_t& ctx, const std::string& socketAddress);
    ~LiveView();

    bool addPicture(const Picture& p);

private:
    void write(const Picture& p);

    using Worker =
        concurrency::SpScQueue<Picture,
            std::function<void(const Picture&)>>;

    zmq::context_t& context_;
    zmq::socket_t socket_;
    std::vector<unsigned char> imageBuffer_;
    std::atomic<bool> running_;
    Worker worker_;
};

} // namespace antifurto

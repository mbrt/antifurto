#pragma once

#include <future>
#include <chrono>
#include <functional>
#include <memory>
#include <string>

#include "ipc/ZmqLazyContext.hpp"

namespace antifurto {

// fwd reference
class Configuration;
class Picture;
class LiveView;

/// This class is responsible in starting and stopping LiveView in a
/// gracefully way
class LiveViewController
{
public:
    /// This callback is called with a parameter 'true' when a registration
    /// is needed, 'false' when it is no more needed
    using PictureRegistrationRequest = std::function<void(bool)>;

    LiveViewController(const Configuration& c, PictureRegistrationRequest regF);
    ~LiveViewController();

    void addPicture(const Picture& p);
    void start();
    void stop();

private:
    PictureRegistrationRequest regFunc_;
    std::string socketPath_;
    std::chrono::seconds timeout_;
    std::unique_ptr<LiveView> liveView_;
    ipc::ZmqContextPtr zmqCtx_;
    std::chrono::system_clock::time_point lastPictureWrittenTime_;
    bool running_ = false;
    std::future<void> stopWork_;
};

} // namespace antifurto

#pragma once

#include "concurrency/TaskScheduler.hpp"
#include "MotionDetector.hpp"
#include "RecordingController.hpp"
#include "NotificationController.hpp"
#include "Picture.hpp"

#include <functional>
#include <chrono>

namespace antifurto {

// fwd decl
class Configuration;

class MonitorController
{
public:
    using SetPicturesInterval = std::function<void(std::chrono::milliseconds)>;

    MonitorController(const Configuration& c, SetPicturesInterval s);
    void examinePicture(const Picture& picture);

private:
    void onAlarmStateChanged(MotionDetector::State state);

    concurrency::TaskScheduler scheduler_;
    MotionDetector motionDetector_;
    RecordingController recording_;
    NotificationController notification_;
    SetPicturesInterval setPicturesInterval_;
};

} // namespace antifurto

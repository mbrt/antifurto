#pragma once

#include "concurrency/TaskScheduler.hpp"
#include "MotionDetector.hpp"
#include "RecordingController.hpp"
#include "MailNotificationController.hpp"
#include "WhatsappNotificationController.hpp"
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

    /// This callback is called with a parameter 'true' when a registration
    /// is needed, 'false' when it is no more needed
    using PictureRegistrationRequest = std::function<void(bool)>;

    MonitorController(const Configuration& c, SetPicturesInterval s,
                      PictureRegistrationRequest r);
    void startMonitor();
    void stopMonitor();
    void examinePicture(const Picture& picture);

private:
    void onAlarmStateChanged(MotionDetector::State state);

    concurrency::TaskScheduler scheduler_;
    MotionDetector motionDetector_;
    RecordingController recording_;
    WhatsappNotificationController whatsapp_;
    MailNotificationController mail_;
    SetPicturesInterval setPicturesInterval_;
    PictureRegistrationRequest pictureRegistrationRequest_;
};

} // namespace antifurto

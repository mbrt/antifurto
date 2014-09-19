#include "MonitorController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "log/Log.hpp"

namespace antifurto {

MonitorController::
MonitorController(Configuration const& c,
                  SetPicturesInterval set,
                  PictureRegistrationRequest r)
    : recording_(c, motionDetector_, scheduler_)
    , notification_(c, motionDetector_, scheduler_)
    , setPicturesInterval_(std::move(set))
    , pictureRegistrationRequest_(r)
{
    motionDetector_.addObserver([this](MotionDetector::State s) {
        onAlarmStateChanged(s);
    });
}

void MonitorController::startMonitor()
{
    motionDetector_.reset();
    pictureRegistrationRequest_(true);
}

void MonitorController::stopMonitor()
{
    pictureRegistrationRequest_(false);
}

void MonitorController::examinePicture(const Picture& p)
{
    recording_.addPicture(p);
    motionDetector_.examinePicture(p);
}

void MonitorController::onAlarmStateChanged(MotionDetector::State state)
{
    using State = MotionDetector::State;

    switch (state) {
    case State::NO_ALARM:
        setPicturesInterval_(config::monitorCycleDuration());
        break;
    case State::PRE_ALARM:
        setPicturesInterval_(config::monitorCycleDurationOnAlarm());
        break;
    default:
        break;
    }
    log::info() << "Alarm state: " << state;
}

} // namespace antifurto

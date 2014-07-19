#include "MonitorController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"

namespace antifurto {

MonitorController::
MonitorController(Configuration const& c, SetPicturesInterval set)
    : recording_(c, motionDetector_, scheduler_)
    , notification_(c, motionDetector_)
    , setPicturesInterval_(std::move(set))
{
    motionDetector_.addObserver([this](MotionDetector::State s) {
        onAlarmStateChanged(s);
    });
}

void MonitorController::performMaintenance()
{
    recording_.performMaintenance();
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
    LOG_INFO << "Alarm state: " << state;
}

} // namespace antifurto

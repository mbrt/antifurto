#include "MonitorController.hpp"
#include "Config.hpp"
#include "MotionDetector.hpp"
#include "RecordingController.hpp"
#include "meta/Metronome.hpp"

#include <thread>

namespace antifurto {

// controller implementation
class MonitorControllerImpl
{
public:
    MonitorControllerImpl()
        : metronome_(config::monitorCycleDuration())
        , recording_(motionDetector_)
        , running_(true)
        , loopThread_([this](){ monitor(); })
    {
        motionDetector_.addObserver([this](MotionDetector::State s) {
            onAlarmStateChanged(s);
        });
    }

    ~MonitorControllerImpl()
    {
        running_ = false;
        loopThread_.join();
    }

private:
    void monitor()
    {
        Picture p;
        while (running_) {
            camera_.takePicture(p);
            recording_.addPicture(p);
            motionDetector_.examinePicture(p);
            metronome_.checkpoint();
        }
    }

    void onAlarmStateChanged(MotionDetector::State state)
    {
        using State = MotionDetector::State;

        switch (state) {
        case State::NO_ALARM:
            metronome_ = Metronome(config::monitorCycleDuration());
            break;
        case State::PRE_ALARM:
            metronome_ = Metronome(config::monitorCycleDurationOnAlarm());
            break;
        default:
            break;
        }
    }

    using Metronome = meta::DefaultMetronome;

    config::Camera camera_;
    MotionDetector motionDetector_;
    Metronome metronome_;
    RecordingController recording_;
    bool running_;
    std::thread loopThread_;
};


MonitorController::MonitorController()
    : pimpl_(new MonitorControllerImpl)
{ }

} // namespace antifurto

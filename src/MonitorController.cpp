#include "MonitorController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "MotionDetector.hpp"
#include "RecordingController.hpp"
#include "NotificationController.hpp"
#include "Log.hpp"
#include "concurrency/TaskScheduler.hpp"
#include "meta/Metronome.hpp"

#include <thread>

namespace antifurto {

// controller implementation
class MonitorControllerImpl
{
public:
    MonitorControllerImpl(Configuration const& c)
        : metronome_(config::monitorCycleDuration())
        , recording_(c, motionDetector_, scheduler_)
        , notification_(c, motionDetector_)
        , startupTimeout_(c.startupTimeout)
        , running_(true)
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

    void performMaintenance()
    {
        recording_.performMaintenance();
    }

    void startMonitor()
    {
        loopThread_ = std::thread([this]{
            std::this_thread::sleep_for(startupTimeout_);
            LOG_INFO << "Monitoring started";
            monitor();
        });
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
            metronome_ = Metronome{config::monitorCycleDuration()};
            break;
        case State::PRE_ALARM:
            metronome_ = Metronome{config::monitorCycleDurationOnAlarm()};
            break;
        default:
            break;
        }
        LOG_INFO << "Alarm state: " << state;
    }

    using Metronome = meta::DefaultMetronome;

    config::Camera camera_;
    concurrency::TaskScheduler scheduler_;
    MotionDetector motionDetector_;
    Metronome metronome_;
    RecordingController recording_;
    NotificationController notification_;
    std::chrono::seconds startupTimeout_;
    bool running_;
    std::thread loopThread_;
};


MonitorController::MonitorController(const Configuration& c)
    : pimpl_(new MonitorControllerImpl(c))
{ }

void MonitorController::performMaintenance()
{
    pimpl_->performMaintenance();
}

void MonitorController::startMonitor()
{
    pimpl_->startMonitor();
}

} // namespace antifurto

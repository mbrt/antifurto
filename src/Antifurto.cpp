#include "Antifurto.hpp"
#include "Config.hpp"
#include "CameraController.hpp"
#include "MonitorController.hpp"
#include "LiveViewController.hpp"
#include "log/Log.hpp"

#include <memory>
#include <mutex>
#include <future>


namespace antifurto {

class AntifurtoImpl
{
public:
    AntifurtoImpl(const Configuration& c, bool maintenanceNeeded)
        : config_(c)
    {
        setPeriodFunction_ = [this](std::chrono::milliseconds t) {
            setMonitorPeriod(t);
        };

        if (maintenanceNeeded) {
            // create a recording controller only for the first maintenance
            RecordingController rec{c};
            rec.performMaintenance();
        }

        if (config_.startup.liveView)
            startLiveView();
        if (config_.startup.monitor)
            startMonitoring();
    }

    ~AntifurtoImpl()
    {
        try {
            stopMonitoring();
            stopLiveView();
        }
        catch (...) { }
    }

    bool isMonitoringActive()
    {
        std::lock_guard<std::mutex> lock{m_};
        return monitorActive_;
    }

    void startMonitoring()
    {
        std::lock_guard<std::mutex> lock{m_};
        if (monitorActive_) return;
        monitorActive_ = true;
        handleCameraControllerNeed();

        stopMonitorRequest_ = false;
        startMonitorFuture_ = std::async(std::launch::async, [this] {
            // wait startup timeout
            int waitedSecs = 0, toWait = config_.startup.monitorTimeout.count();
            log::info() << "Wait " << toWait << " seconds before start monitor";
            while (waitedSecs < toWait && !stopMonitorRequest_) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                ++waitedSecs;
            }

            // check for shutdown request
            std::lock_guard<std::mutex> lock{m_};
            if (stopMonitorRequest_) {
                stopMonitorRequest_ = false;
                return;
            }

            if (!monitor_) {
                monitor_.reset(new MonitorController{
                                   config_, setPeriodFunction_,
                                   [&](bool reg) {
                    if (reg)
                        registerMonitor();
                    else
                        monitorRegistration_.clear();
                }});
            }
            monitor_->startMonitor();
            log::info() << "Monitoring started";
        });
    }

    void stopMonitoring()
    {
        bool waitStartedNeeded = false;
        {
            std::lock_guard<std::mutex> lock{m_};
            if (!monitorActive_) return;
            monitorActive_ = false;

            if (startMonitorFuture_.valid()) {
                log::info() << "Cancel start monitoring";
                stopMonitorRequest_ = true;
                waitStartedNeeded = true;
            }
        }

        // this must be done unlocked
        if (waitStartedNeeded)
            startMonitorFuture_.get();

        std::lock_guard<std::mutex> lock{m_};
        if (monitor_) {
            log::info() << "Stopping monitoring";
            monitor_->stopMonitor();
            log::info() << "Monitoring stopped";
        }
        handleCameraControllerNeed();
    }

    bool isLiveViewActive()
    {
        std::lock_guard<std::mutex> lock{m_};
        return liveViewActive_;
    }

    void startLiveView()
    {
        std::lock_guard<std::mutex> lock{m_};
        if (liveViewActive_) return;
        liveViewActive_ = true;
        handleCameraControllerNeed();

        log::info() << "Start live view";
        setMonitorPeriod(config::liveViewCycleDuration());
        if (!liveView_) {
            liveView_.reset(new LiveViewController(config_, [&](bool reg) {
                if (reg)
                    registerLiveView();
                else {
                    liveViewRegistration_.clear();
                    liveViewActive_ = false;
                    handleCameraControllerNeed();
                }
            }));
        }
        liveView_->start();
    }

    void stopLiveView()
    {
        std::lock_guard<std::mutex> lock{m_};
        if (!liveViewActive_) return;
        liveViewActive_ = false;
        liveView_->stop();
    }

private:
    void setMonitorPeriod(std::chrono::milliseconds t)
    {
        camera_->setDesiredPeriod(monitorRegistration_, t);
    }

    void handleCameraControllerNeed()
    {
        if ((liveViewActive_ || monitorActive_) && !camera_)
            camera_.reset(new CameraController());
// FIXME: when the PiCamera can be safely destroyed
//        you can enable the following lines
#if !defined(ANTIFURTO_RASPBERRY)
        else if (!liveViewActive_ && !monitorActive_)
            camera_.reset();
#endif
    }

    void registerLiveView()
    {
        liveViewRegistration_ = camera_->addObserver([&](const Picture& p) {
            liveView_->addPicture(p);
        }, config::liveViewCycleDuration());
    }

    void registerMonitor()
    {
        monitorRegistration_ = camera_->addObserver([&](const Picture& p){
            monitor_->examinePicture(p);
        }, config::monitorCycleDuration());
    }

    Configuration config_;
    std::unique_ptr<CameraController> camera_;
    std::unique_ptr<MonitorController> monitor_;
    MonitorController::SetPicturesInterval setPeriodFunction_;
    CameraController::Registration monitorRegistration_;
    std::unique_ptr<LiveViewController> liveView_;
    CameraController::Registration liveViewRegistration_;
    std::mutex m_;
    bool stopMonitorRequest_ = false;
    std::future<void> startMonitorFuture_;
    bool liveViewActive_ = false;
    bool monitorActive_ = false;
};


Antifurto::Antifurto(const Configuration& c, bool maintenanceNeeded)
    : pimpl_(new AntifurtoImpl(c, maintenanceNeeded))
{ }

void Antifurto::startMonitoring()
{
    pimpl_->startMonitoring();
}

void Antifurto::stopMonitoring()
{
    pimpl_->stopMonitoring();
}

void Antifurto::startLiveView()
{
    pimpl_->startLiveView();
}

void Antifurto::stopLiveView()
{
    pimpl_->stopLiveView();
}

} // namespace antifurto

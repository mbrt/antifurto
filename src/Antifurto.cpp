#include "Antifurto.hpp"
#include "Config.hpp"
#include "CameraController.hpp"
#include "MonitorController.hpp"
#include "LiveView.hpp"
#include "Log.hpp"

#include <memory>
#include <fstream>
#include <iterator>
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
            // create a monitor only for the first maintenance
            MonitorController monitor{c, setPeriodFunction_};
            monitor.performMaintenance();
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
            LOG_INFO << "Wait " << toWait << " seconds before start monitor";
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

            LOG_INFO << "Monitoring started";
            monitor_.reset(new MonitorController{config_, setPeriodFunction_});
            monitorRegistration_ = camera_->addObserver([&](const Picture& p){
                monitor_->examinePicture(p);
            }, config::monitorCycleDuration());
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
                LOG_INFO << "Cancel start monitoring";
                stopMonitorRequest_ = true;
                waitStartedNeeded = true;
            }
        }

        // this must be done unlocked
        if (waitStartedNeeded)
            startMonitorFuture_.get();

        std::lock_guard<std::mutex> lock{m_};
        if (monitor_) {
            LOG_INFO << "Stopping monitoring";
            monitorRegistration_.clear();
            monitor_.reset();
            LOG_INFO << "Monitoring stopped";
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

        LOG_INFO << "Start live view";
        setMonitorPeriod(config::liveViewCycleDuration());
        liveView_.reset(new LiveView("/tmp/antifurto/live", 3));
        liveViewRegistration_ = camera_->addObserver([&](const Picture& p) {
            liveView_->addPicture(p);
        }, config::liveViewCycleDuration());
    }

    void stopLiveView()
    {
        std::lock_guard<std::mutex> lock{m_};
        if (!liveViewActive_) return;
        liveViewActive_ = false;

        LOG_INFO << "Stopping live view";
        liveViewRegistration_.clear();
        auto token = std::async(std::launch::async, [this] {
            liveView_.reset();
        });
        if (token.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
            // consume the current
            std::ifstream f{liveView_->getCurrentFilename().c_str()};
            std::istream_iterator<uint8_t> it(f), end;
            while (it != end)
                ++it;
        }
        token.get();
        handleCameraControllerNeed();
        LOG_INFO << "Live view stopped";

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
        else if (!liveViewActive_ && !monitorActive_)
            camera_.reset();
    }

    Configuration config_;
    std::unique_ptr<CameraController> camera_;
    std::unique_ptr<MonitorController> monitor_;
    MonitorController::SetPicturesInterval setPeriodFunction_;
    CameraController::Registration monitorRegistration_;
    std::unique_ptr<LiveView> liveView_;
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
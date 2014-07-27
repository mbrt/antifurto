#include "Antifurto.hpp"
#include "Config.hpp"
#include "CameraController.hpp"
#include "MonitorController.hpp"
#include "LiveView.hpp"
#include "Log.hpp"

#include <memory>
#include <fstream>
#include <iterator>


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

    void startMonitoring()
    {
        // TODO: better to use another method here, because the caller to
        // start must wait for the timeout
        std::this_thread::sleep_for(config_.startup.monitorTimeout);
        LOG_INFO << "Monitoring started";
        monitor_.reset(new MonitorController{config_, setPeriodFunction_});
        monitorRegistration_ = camera_.addObserver([&](const Picture& p){
            monitor_->examinePicture(p);
        }, config::monitorCycleDuration());
    }

    void stopMonitoring()
    {
        LOG_INFO << "Stopping monitoring";
        monitorRegistration_.clear();
        monitor_.reset();
        LOG_INFO << "Monitoring stopped";
    }

    void startLiveView()
    {
        LOG_INFO << "Start live view";
        setMonitorPeriod(config::liveViewCycleDuration());
        liveView_.reset(new LiveView("/tmp/antifurto/live", 3));
        liveViewRegistration_ = camera_.addObserver([&](const Picture& p){
            liveView_->addPicture(p);
        }, config::liveViewCycleDuration());
    }

    void stopLiveView()
    {
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
        LOG_INFO << "Live view stopped";

    }

private:
    void setMonitorPeriod(std::chrono::milliseconds t)
    {
        camera_.setDesiredPeriod(monitorRegistration_, t);
    }

    Configuration config_;
    CameraController camera_;
    std::unique_ptr<MonitorController> monitor_;
    MonitorController::SetPicturesInterval setPeriodFunction_;
    CameraController::Registration monitorRegistration_;
    std::unique_ptr<LiveView> liveView_;
    CameraController::Registration liveViewRegistration_;
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

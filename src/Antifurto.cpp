#include "Antifurto.hpp"
#include "Config.hpp"
#include "CameraController.hpp"
#include "MonitorController.hpp"

#include <memory>


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
    }

    void startMonitoring()
    {
        // TODO: better to use another method here, because the caller to
        // start must wait for the timeout
        std::this_thread::sleep_for(config_.startupTimeout);
        setMonitorPeriod(config::monitorCycleDuration());
        monitor_.reset(new MonitorController{config_, setPeriodFunction_});
        monitorRegistration_ = camera_.addObserver([&](const Picture& p){
            monitor_->examinePicture(p); });
    }

    void stopMonitoring()
    {
        monitorRegistration_.clear();
        monitor_.reset();
    }

    void startLiveView(){}
    void stopLiveView(){}

private:
    void setMonitorPeriod(std::chrono::milliseconds t)
    {
        camera_.setPeriod(t);
    }

    Configuration config_;
    CameraController camera_;
    std::unique_ptr<MonitorController> monitor_;
    MonitorController::SetPicturesInterval setPeriodFunction_;
    CameraController::Registration monitorRegistration_;
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

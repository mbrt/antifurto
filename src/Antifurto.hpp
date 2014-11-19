#pragma once

#include "meta/SmartPtr.hpp"

namespace antifurto {

class AntifurtoImpl;
class Configuration;

class Antifurto
{
public:
    enum class ServiceStatus {
        RUNNING = 0,
        STOPPED = 1,
        STARTING = 2,
        STOPPING = 3,
        UNAVAILABLE = 4
    };

    Antifurto(const Configuration& c, bool maintenanceNeeded = true);

    void startMonitoring();
    void stopMonitoring();
    ServiceStatus getMonitoringStatus() const;

    void startLiveView();
    void stopLiveView();
    ServiceStatus getLiveViewStatus() const;

private:
    meta::ErasedUniquePtr<AntifurtoImpl> pimpl_;
};

} // namespace antifurto

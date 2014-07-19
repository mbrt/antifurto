#pragma once

#include "meta/SmartPtr.hpp"

namespace antifurto {

class AntifurtoImpl;
class Configuration;

class Antifurto
{
public:
    Antifurto(const Configuration& c, bool maintenanceNeeded = true);

    void startMonitoring();
    void stopMonitoring();

    void startLiveView();
    void stopLiveView();

private:
    meta::ErasedUniquePtr<AntifurtoImpl> pimpl_;
};

} // namespace antifurto

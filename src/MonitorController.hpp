#pragma once

#include "meta/SmartPtr.hpp"

namespace antifurto {

// fwd decl
class MonitorControllerImpl;
class Configuration;

class MonitorController
{
public:
    MonitorController(const Configuration& c);

private:
    meta::ErasedUniquePtr<MonitorControllerImpl> pimpl_;
};

} // namespace antifurto

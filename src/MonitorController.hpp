#pragma once

#include "meta/SmartPtr.hpp"
#include "Config_fwd.hpp"

namespace antifurto {

// fwd decl
class MonitorControllerImpl;

class MonitorController
{
public:
    MonitorController(config::Configuration& c);

private:
    meta::ErasedUniquePtr<MonitorControllerImpl> pimpl_;
};

} // namespace antifurto

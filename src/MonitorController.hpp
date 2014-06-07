#pragma once

#include "meta/SmartPtr.hpp"

namespace antifurto {

// fwd decl
class MonitorControllerImpl;

class MonitorController
{
public:
    MonitorController();

private:
    meta::ErasedUniquePtr<MonitorControllerImpl> pimpl_;
};

} // namespace antifurto

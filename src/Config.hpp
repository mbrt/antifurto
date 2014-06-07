#pragma once

#if defined(ANTIFURTO_RASPBERRY)
#   include "PiCamera.hpp"
#else
#   include "CvCamera.hpp"
#endif

#include <chrono>

namespace antifurto {

/// Configuration class
class Config
{
public:
#if defined(ANTIFURTO_RASPBERRY)
    using Camera = antifurto::PiCamera;
#else
    using Camera = antifurto::CvCamera;
#endif
    static constexpr std::chrono::milliseconds monitorCycleDuration { 500 };
    static constexpr std::chrono::milliseconds monitorCycleDurationOnAlarm { 100 };
};

} // namespace antifurto

#pragma once

#if defined(ANTIFURTO_RASPBERRY)
#   include "PiCamera.hpp"
#else
#   include "CvCamera.hpp"
#endif

#include <chrono>

namespace antifurto {
namespace config {

#if defined(ANTIFURTO_RASPBERRY)
    using Camera = antifurto::PiCamera;
#else
    using Camera = antifurto::CvCamera;
#endif

using Milliseconds = std::chrono::milliseconds;
constexpr Milliseconds monitorCycleDuration() { return Milliseconds{ 500 }; }
constexpr Milliseconds monitorCycleDurationOnAlarm() { return Milliseconds{ 100 }; }

constexpr const char* configDir() { return "config"; }
constexpr const char* archiveDir() { return "/tmp"; }

} // namespace config
} // namespace antifurto

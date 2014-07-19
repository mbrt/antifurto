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
using Minutes = std::chrono::minutes;
constexpr Milliseconds monitorCycleDuration() { return Milliseconds{ 500 }; }
constexpr Milliseconds monitorCycleDurationOnAlarm() { return Milliseconds{ 200 }; }
constexpr Milliseconds liveViewCycleDuration() { return Milliseconds{ 200 }; }
constexpr Minutes minNotificationDelay() { return Minutes{ 10 }; }

std::string exeDir();
std::string configDir();
std::string configFile();

constexpr const char* archiveDir() { return "/tmp/antifurto/archive"; }
constexpr const char* logDir() { return "/tmp/antifurto/log"; }
constexpr const char* tmpDir() { return "/tmp"; }

constexpr unsigned int maxArchiveDays() { return 7; }

}} // namespace antifurto::config

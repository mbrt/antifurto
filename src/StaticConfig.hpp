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
using Seconds = std::chrono::seconds;
constexpr Milliseconds monitorCycleDuration() { return Milliseconds{ 500 }; }
constexpr Milliseconds monitorCycleDurationOnAlarm() { return Milliseconds{ 200 }; }
constexpr Seconds minNotificationDelay() { return Seconds{ 15 }; }

constexpr const char* configDir() { return "config"; }
constexpr const char* archiveDir() { return "/tmp/antifurto/archive"; }
constexpr const char* logDir() { return "/tmp/antifurto/log"; }
constexpr const char* tmpDir() { return "/tmp"; }
constexpr const char* configFile() { return "config/antifurto.cfg"; }
constexpr const char* dropboxConfigFile() { return "config/dropbox.cfg"; }
constexpr const char* whatsappConfigFile() { return "config/whatsapp.cfg"; }

constexpr unsigned int maxArchiveDays() { return 7; }

}} // namespace antifurto::config

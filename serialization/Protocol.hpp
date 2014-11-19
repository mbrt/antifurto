#pragma once

#include <stdint.h>

namespace antifurto {
namespace serialization {

enum class MessageType : int32_t
{
    LIVE_VIEW_REQUEST = 1,
    LIVE_VIEW_REPLY = 2,
    MONITOR_STATUS_REQUEST = 3,
    MONITOR_STATUS_REPLY = 4,
    ERROR_UNKNOWN_MESSAGE = 5,
    MAX // Keep this as last value
};

enum class ServiceStatus : int32_t
{
    RUNNING = 0,
    STOPPED = 1,
    STARTING = 2,
    STOPPING = 3,
    UNAVAILABLE = 4
};

struct MonitorStatusReply
{
    ServiceStatus status;
};

}} // namespace antifurto::serialization

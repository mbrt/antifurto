#pragma once

namespace antifurto {
namespace serialization {

enum class MessageType
{
    LIVE_VIEW_REQUEST = 1,
    LIVE_VIEW_REPLY = 2,
    MONITOR_STATUS_REQUEST = 3,
    MONITOR_STATUS_REPLY = 4,
    MAX // Keep this as last value
};

enum class ServiceStatus
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

#pragma once

namespace antifurto {
namespace serialization {

enum class MessageType
{
    LiveViewRequest = 1,
    LiveViewReply = 2,
    MonitorStatusRequest = 3,
    MonitorStatusReply = 4,
    Max // Keep this as last value
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

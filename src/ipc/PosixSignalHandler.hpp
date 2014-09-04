#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <chrono>

namespace antifurto {
namespace ipc {

constexpr std::chrono::milliseconds defaultPollingTime()
{ return std::chrono::milliseconds{1000}; }

/// This class handles Linux signals
///
/// Note that the class must be initialized before any thread creation.
/// Before the call to enterSignalHandlingLoop, no signals are handled.
class PosixSignalHandler
{
public:
    PosixSignalHandler(std::chrono::milliseconds pollingTime = defaultPollingTime());
    PosixSignalHandler(const PosixSignalHandler& ) = delete;
    PosixSignalHandler& operator =(const PosixSignalHandler& ) = delete;

    using Handler = std::function<void(int)>;

    void setSignalHandler(int signal, Handler h);
    void ignoreSignal(int signal);
    void enterSignalHandlingLoop();
    void leaveSignalHandlingLoop();

private:
    void setSignalHandlers();
    void signalHandlingLoop();

    const std::chrono::milliseconds pollingTime_;
    std::vector<Handler> handlerList_;
    std::atomic<bool> run_;
};

} // namespace ipc
} // namespace antifurto

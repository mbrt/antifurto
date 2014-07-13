#pragma once

#include "Exception.hpp"

#include <functional>
#include <vector>
#include <atomic>

namespace antifurto {
namespace ipc {

/// This class handles Linux signals
///
/// Note that the class must be initialized before any thread creation.
/// Before the call to enterSignalHandlingLoop, no signals are handled.
class PosixSignalHandler
{
public:
    PosixSignalHandler(std::initializer_list<int> signs);
    PosixSignalHandler(const PosixSignalHandler& ) = delete;
    PosixSignalHandler& operator =(const PosixSignalHandler& ) = delete;

    using Handler = std::function<void(int)>;

    void setSignalHandler(int signal, Handler h);
    void enterSignalHandlingLoop();
    void leaveSignalHandlingLoop();

private:
    std::vector<Handler> handlerList_;
    std::vector<int> signalsToBeHandled_;
    std::atomic<bool> run_;
};

} // namespace ipc
} // namespace antifurto

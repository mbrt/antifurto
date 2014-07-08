#pragma once

#include "Exception.hpp"

#include <functional>
#include <vector>
#include <mutex>

namespace antifurto {
namespace concurrency {

/// This class handles Linux signals
///
/// Note that the class must be initialized as first instruction of the main
/// and the enterSignalHandlingLoop function must be called at last.
class PosixSignalHandler
{
public:
    PosixSignalHandler();
    ~PosixSignalHandler();
    PosixSignalHandler(const PosixSignalHandler& ) = delete;
    PosixSignalHandler& operator =(const PosixSignalHandler& ) = delete;

    using Handler = std::function<void(int)>;

    void setSignalHandler(int signal, Handler h);
    void enterSignalHandlingLoop();

private:
    void clearSignalMask();

    std::mutex handlerListM_;
    std::vector<Handler> handlerList_;
    bool loopRunning_;
};

} // namespace concurrency
} // namespace antifurto

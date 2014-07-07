#include "PosixSignalHandler.hpp"

#include <csignal>

namespace antifurto {
namespace concurrency {
namespace { // anon

PosixSignalHandler* instance = nullptr;

}

// anon namespace

PosixSignalHandler::PosixSignalHandler()
    : handlerList_(SIGRTMAX)
{
    if (instance)
        throw Exception("Cannot initialize signal handler multiple times");
    instance = this;
    clearSignalMask();
}

void PosixSignalHandler::setSignalHandler(int signal, Handler h)
{
    handlerList_[signal] = std::move(h);
}

PosixSignalHandler::~PosixSignalHandler()
{
    instance = nullptr;
}


void PosixSignalHandler::clearSignalMask()
{
    sigset_t signalMask;
    ::sigemptyset(&signalMask);
    if (::pthread_sigmask(SIG_BLOCK, &signalMask, NULL) != 0)
        throw Exception("Cannot block signals");
}

} // namespace concurrency
} // namespace antifurto

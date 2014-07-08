#include "PosixSignalHandler.hpp"

#include <csignal>
#include "../text/ToString.hpp"

// see http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_sigmask.html
// for an implementation example

namespace antifurto {
namespace concurrency {
namespace { // anon

PosixSignalHandler* instance = nullptr;

}

// anon namespace

PosixSignalHandler::PosixSignalHandler()
    : handlerList_(SIGRTMAX)
    , loopRunning_(false)
{
    if (instance)
        throw Exception("Cannot initialize signal handler multiple times");
    instance = this;
    clearSignalMask();
}

void PosixSignalHandler::setSignalHandler(int signal, Handler h)
{
    std::lock_guard<std::mutex> lock(handlerListM_);
    if (loopRunning_)
        throw Exception("Cannot set signal handler after handling started");
    handlerList_[signal] = std::move(h);
}

void PosixSignalHandler::enterSignalHandlingLoop()
{
    {
        std::lock_guard<std::mutex> lock(handlerListM_);
        loopRunning_ = true;
    }

    int signum = 0;
    sigset_t signalMask;
    ::sigemptyset(&signalMask);
    for (auto& handler : handlerList_) {
        if (handler)
            ::sigaddset(&signalMask, signum);
        ++signum;
    }
    if (::pthread_sigmask(SIG_SETMASK, &signalMask, NULL) != 0)
        throw Exception("Cannot set signal mask");

    while (true) {
        siginfo_t info;
        if (::sigwaitinfo(&signalMask, &info) == -1)
            throw Exception("Error waiting for signal");
        int signum = info.si_signo;
        Handler& h = handlerList_[signum];
        if (!h)
            throw Exception(text::toString("Unexpected signal ", signum));
        h(signum);
        if (signum == SIGTERM || signum == SIGINT)
            return;
    }
}

PosixSignalHandler::~PosixSignalHandler()
{
    instance = nullptr;
}


void PosixSignalHandler::clearSignalMask()
{
    sigset_t signalMask;
    ::sigfillset(&signalMask);
    if (::pthread_sigmask(SIG_BLOCK, &signalMask, NULL) != 0)
        throw Exception("Cannot block signals");
}

} // namespace concurrency
} // namespace antifurto

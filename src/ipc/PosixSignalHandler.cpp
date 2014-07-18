#include "PosixSignalHandler.hpp"

#include <csignal>
#include <algorithm>

#include "../text/ToString.hpp"

// see http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_sigmask.html
// for an implementation example

namespace antifurto {
namespace ipc {
namespace { // anon

template <typename Container>
sigset_t addToSigset(Container signs)
{
    sigset_t result;
    ::sigemptyset(&result);
    for (auto s : signs)
        ::sigaddset(&result, s);
    return result;
}

} // anon namespace

PosixSignalHandler::PosixSignalHandler(std::initializer_list<int> signs)
    : handlerList_(SIGRTMAX)
    , signalsToBeHandled_(signs)
    , run_(false)
{
    std::sort(std::begin(signalsToBeHandled_), std::end(signalsToBeHandled_));
    sigset_t signalMask = addToSigset(signalsToBeHandled_);
    if (::pthread_sigmask(SIG_BLOCK, &signalMask, NULL) != 0)
        throw Exception("Cannot block signals");
}

void PosixSignalHandler::setSignalHandler(int signal, Handler h)
{
    if (!std::binary_search(std::begin(signalsToBeHandled_), std::end(signalsToBeHandled_), signal))
        throw Exception(text::toString(
                "Cannot add signal ", signal,
                ", as it was not specified in construction"));
    handlerList_[signal] = std::move(h);
}

void PosixSignalHandler::enterSignalHandlingLoop()
{
    run_.store(true, std::memory_order_release);

    sigset_t signalMask = addToSigset(signalsToBeHandled_);
    signalsToBeHandled_.clear(); // signals handled not needed anymore

    while (run_.load(std::memory_order_acquire)) {
        siginfo_t info;
        if (::sigwaitinfo(&signalMask, &info) == -1)
            throw Exception("Error waiting for signal");
        int signum = info.si_signo;
        Handler& h = handlerList_[signum];
        if (!h)
            throw Exception(text::toString("Unexpected signal ", signum));
        h(signum);
    }
}

void PosixSignalHandler::leaveSignalHandlingLoop()
{
    run_.store(false, std::memory_order_release);
}

} // namespace ipc
} // namespace antifurto

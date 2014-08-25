#include "PosixSignalHandler.hpp"

#include <csignal>
#include <algorithm>

#include "../text/ToString.hpp"
#include "../Log.hpp"

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

PosixSignalHandler::PosixSignalHandler(std::vector<int> signs)
    : handlerList_(SIGRTMAX)
    , signalsToBeHandled_(signs)
    , run_(false)
{
    std::sort(std::begin(signalsToBeHandled_), std::end(signalsToBeHandled_));
    sigset_t signalMask = addToSigset(signalsToBeHandled_);
    if (::pthread_sigmask(SIG_BLOCK, &signalMask, NULL) != 0)
        throw Exception("Cannot block signals");
}

PosixSignalHandler::PosixSignalHandler(std::initializer_list<int> signs)
    : PosixSignalHandler(signs)
{ }

void PosixSignalHandler::setSignalHandler(int signal, Handler h)
{
    if (!std::binary_search(std::begin(signalsToBeHandled_),
            std::end(signalsToBeHandled_), signal))
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
    siginfo_t info;

    while (run_.load(std::memory_order_acquire)) {
        if (::sigwaitinfo(&signalMask, &info) == -1)
            throw Exception("Error waiting for signal");
        ::sigprocmask(SIG_BLOCK, &signalMask, nullptr); // block the signals now

        int signum = info.si_signo;
        Handler& h = handlerList_[signum];
        if (h)
            h(signum);
        else
            LOG_WARNING << "Got signal without handler ("
                        << signum << ") ignored";

        ::sigprocmask(SIG_UNBLOCK, &signalMask, nullptr); // unblock signals
    }
}

void PosixSignalHandler::leaveSignalHandlingLoop()
{
    run_.store(false, std::memory_order_release);
}

} // namespace ipc
} // namespace antifurto

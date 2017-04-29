#include "PosixSignalHandler.hpp"
#include "Exception.hpp"
#include "../log/Log.hpp"
#include "../text/ToString.hpp"

#include <csignal>
#include <cstring>
#include <algorithm>
#include <thread>

namespace antifurto {
namespace ipc {
namespace { // anon

std::vector<std::atomic<bool>> signalsToBeHandled(SIGRTMAX);

} // anon namespace

void sigactionHandler(int sig, siginfo_t* , void* )
{
    signalsToBeHandled[sig].store(true, std::memory_order_release);
}


PosixSignalHandler::PosixSignalHandler(std::chrono::milliseconds pollingTime)
    : pollingTime_(pollingTime)
    , handlerList_(SIGRTMAX)
    , run_(false)
{
    for (auto& flag : signalsToBeHandled)
        flag.store(false, std::memory_order_release);
}

void PosixSignalHandler::setSignalHandler(int signal, Handler h)
{
    if (run_.load(std::memory_order_acquire))
        throw Exception("Cannot add signal while loop is running");
    handlerList_[signal] = std::move(h);
}

void PosixSignalHandler::ignoreSignal(int signal)
{
    ::signal(signal, SIG_IGN);
}

void PosixSignalHandler::enterSignalHandlingLoop()
{
    run_.store(true, std::memory_order_release);
    setSignalHandlers();
    signalHandlingLoop();
}

void PosixSignalHandler::leaveSignalHandlingLoop()
{
    run_.store(false, std::memory_order_release);
}


void PosixSignalHandler::setSignalHandlers()
{
    struct ::sigaction act;
    ::memset(&act, 0, sizeof(act));
    act.sa_sigaction = sigactionHandler;
    ::sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART | SA_SIGINFO;

    int s = 0;
    for (auto& h : handlerList_) {
        if (h)
            ::sigaction(s, &act, nullptr);
        ++s;
    }
}

void PosixSignalHandler::signalHandlingLoop()
{
    while (run_.load(std::memory_order_acquire)) {
        int signum = -1;
        for (auto& s : signalsToBeHandled) {
            ++signum;
            if (!s.load(std::memory_order_relaxed)) continue;

            s.store(false, std::memory_order_relaxed);
            if (handlerList_[signum])
                handlerList_[signum](signum);
            else
                log::warning() << "Got signal without handler ("
                               << signum << ") ignored";
        }
        std::this_thread::sleep_for(pollingTime_);
    }
}

} // namespace ipc
} // namespace antifurto

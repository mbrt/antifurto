#include "SignalHandler.hpp"
#include "Log.hpp"
#include "Antifurto.hpp"
#include "ipc/PosixSignalHandler.hpp"

#include <signal.h>

namespace antifurto {

SignalHandler::SignalList SignalHandler::getSignalsNeeded()
{
    return { SIGTERM, SIGINT, SIGUSR1, SIGUSR2 };
}

SignalHandler::
SignalHandler(ipc::PosixSignalHandler& handler, Antifurto& controller)
    : handler_(handler), controller_(controller)
{
    auto signalHandler = [&](int signal){
        onSignal(signal);
    };
    for (int sig : getSignalsNeeded())
        handler_.setSignalHandler(sig, signalHandler);
}

void SignalHandler::onSignal(int signal)
{
    LOG_INFO << "Received signal " << signal;
    switch (signal) {
    case SIGTERM:
    case SIGINT:
        {
            LOG_INFO << "Shutting down...";
            handler_.leaveSignalHandlingLoop();
        }
        break;
    case SIGUSR1:
        controller_.startMonitoring();
        break;
    case SIGUSR2:
        controller_.stopMonitoring();
        break;
    default:
        LOG_ERROR << "Unexpected signal";
    }
}


} // namespace antifurto

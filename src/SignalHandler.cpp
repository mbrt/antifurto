#include "SignalHandler.hpp"
#include "Antifurto.hpp"
#include "ipc/PosixSignalHandler.hpp"
#include "log/Log.hpp"

#include <signal.h>

namespace antifurto {

SignalHandler::SignalList SignalHandler::getSignalsNeeded()
{
    return { SIGTERM, SIGINT, SIGUSR1, SIGUSR2, SIGRTMIN + 3, SIGRTMIN + 4 };
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
    log::info() << "Received signal " << signal;
    switch (signal) {
    case SIGTERM:
    case SIGINT:
        {
            log::info() << "Shutting down...";
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
        {
            // required if else because SIGRTMIN is not a constant expression
            if (signal == SIGRTMIN + 3)
                controller_.startLiveView();
            else if (signal == SIGRTMIN + 4)
                controller_.stopLiveView();
            else
                log::error() << "Unexpected signal";
        }
        break;
    }
}


} // namespace antifurto

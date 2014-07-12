#include "concurrency/PosixSignalHandler.hpp"

#include <iostream>
#include <thread>
#include <signal.h>

using namespace antifurto::concurrency;


void handleSignal(int sig)
{
    std::cout << "Caught signal " << sig << std::endl;
}

int main()
{
    PosixSignalHandler handler{SIGUSR1, SIGTERM, SIGINT};
    handler.setSignalHandler(SIGUSR1, &handleSignal);
    handler.setSignalHandler(SIGTERM, [&](int signal){
        handleSignal(signal);
        handler.leaveSignalHandlingLoop();
    });
    handler.setSignalHandler(SIGINT, [&](int signal){
        handleSignal(signal);
        handler.leaveSignalHandlingLoop();
    });
    handler.enterSignalHandlingLoop();
    return 0;
}

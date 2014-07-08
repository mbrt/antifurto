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
    PosixSignalHandler handler;
    handler.setSignalHandler(SIGUSR1, &handleSignal);
    handler.setSignalHandler(SIGTERM, &handleSignal);
    handler.enterSignalHandlingLoop();
    return 0;
}

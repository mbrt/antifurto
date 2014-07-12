#include "concurrency/PosixSignalHandler.hpp"
#include "ipc/Fork.hpp"

#include <iostream>
#include <thread>
#include <signal.h>
#include <boost/test/minimal.hpp>

using namespace antifurto;
using namespace antifurto::concurrency;


void handleSignal(int sig)
{
    std::cout << "Caught signal " << sig << std::endl;
}

int termTest()
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

int test_main(int, char**)
{
    BOOST_CHECK(ipc::forkAndCall(termTest) == 0);
    return 0;
}

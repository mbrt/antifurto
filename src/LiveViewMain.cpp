#include "Antifurto.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"
#include "ipc/PosixSignalHandler.hpp"

#include <thread>
#include <iostream>
#include <signal.h>

using namespace antifurto;

int main(int argc, char* argv[])
{
    {   // this block force destruction before logging exit
        ipc::PosixSignalHandler signalHandler{SIGTERM, SIGINT};
        auto termSignalHandler = [&](int signal){
            LOG_INFO << "Received signal " << signal << ". Shutting down...";
            signalHandler.leaveSignalHandlingLoop();
        };
        signalHandler.setSignalHandler(SIGTERM, termSignalHandler);
        signalHandler.setSignalHandler(SIGINT, termSignalHandler);

        ConfigurationParser parser;
        parser.parseCmdLine(argc, argv);
        parser.parseConfigFile(config::configFile().c_str());
        if (!parser.canContinue())
            return 1;

        initLogger(parser.getConfiguration());
        LOG_INFO << "Init of live view";
        Antifurto app{parser.getConfiguration()};
        app.startLiveView();

        signalHandler.enterSignalHandlingLoop();
        app.stopLiveView();
    }

    LOG_INFO << "Exit now";
    return 0;
}

#include "Antifurto.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"
#include "SignalHandler.hpp"
#include "ipc/PosixSignalHandler.hpp"

#include <thread>
#include <iostream>

using namespace antifurto;

int main(int argc, char* argv[])
{
    {   // this block force destruction before logging exit
        ipc::PosixSignalHandler signalHandler;

        ConfigurationParser parser;
        try {
            parser.parseCmdLine(argc, argv);
            parser.parseConfigFile(config::configFile().c_str());
        }
        catch (std::exception& e) {
            LOG_ERROR << "Error in config file or command line params: "
                      << e.what();
            return EXIT_FAILURE;
        }

        if (!parser.canContinue())
            return 1;

        initLogger(parser.getConfiguration());
        LOG_INFO << "Init of antifurto";
        Antifurto app{parser.getConfiguration()};
        SignalHandler appSignalHandler{signalHandler, app};

        signalHandler.enterSignalHandlingLoop();
        //std::this_thread::sleep_for(std::chrono::hours(1));
    }

    LOG_INFO << "Exit now";
    return EXIT_SUCCESS;
}

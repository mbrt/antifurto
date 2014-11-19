#include "Antifurto.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "SignalHandler.hpp"
#include "QueryStatus.hpp"
#include "ipc/PosixSignalHandler.hpp"
#include "log/Log.hpp"

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
            log::error() << "Error in config file or command line params: "
                         << e.what();
            return EXIT_FAILURE;
        }

        if (!parser.canContinue())
            return 1;

        log::init(parser.getConfiguration());
        log::info() << "Init of antifurto";
        Antifurto app{parser.getConfiguration()};
        SignalHandler appSignalHandler{signalHandler, app};
        AntifurtoQuery query{app, parser.getConfiguration()};

        query.start();
        signalHandler.enterSignalHandlingLoop();
        //std::this_thread::sleep_for(std::chrono::hours(1));
    }

    log::info() << "Exit now";
    return EXIT_SUCCESS;
}

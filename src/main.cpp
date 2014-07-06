#include "MonitorController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"
#include "Log.hpp"

#include <thread>
#include <iostream>

using namespace antifurto;

int main(int argc, char* argv[])
{
    ConfigurationParser parser;
    parser.parseCmdLine(argc, argv);
    parser.parseConfigFile(config::configFile().c_str());
    if (!parser.canContinue())
        return 1;

    initLogger(parser.getConfiguration());
    LOG_INFO << "Init of antifurto";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    MonitorController controller(parser.getConfiguration());
    controller.performMaintenance();
    controller.startMonitor();
    //std::this_thread::sleep_for(std::chrono::hours(1));
    std::cout << "Monitoring started.\n"
                 "Press any key to stop...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}

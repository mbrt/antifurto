#include "MonitorController.hpp"
#include "Config.hpp"
#include "StaticConfig.hpp"

#include <thread>
#include <iostream>

using namespace antifurto;

int main(int argc, char* argv[])
{
    ConfigurationParser parser;
    parser.parseCmdLine(argc, argv);
    parser.parseConfigFile(config::configFile());
    if (!parser.canContinue())
        return 1;

    std::this_thread::sleep_for(std::chrono::seconds(5));
    MonitorController controller(parser.getConfiguration());
    //std::this_thread::sleep_for(std::chrono::seconds(1000));
    std::cout << "Monitoring started.\n"
                 "Press any key to stop...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}

#include "MonitorController.hpp"
#include "Config.hpp"

#include <thread>
#include <iostream>

using namespace antifurto;

int main(int, char*[])
{
    std::this_thread::sleep_for(std::chrono::seconds(5));
    Configuration cfg(config::configFile());
    MonitorController controller(cfg);
    //std::this_thread::sleep_for(std::chrono::seconds(1000));
    std::cout << "Monitoring started.\n"
                 "Press any key to stop...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}

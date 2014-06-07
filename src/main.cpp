#include "MonitorController.hpp"

#include <thread>
#include <iostream>

using namespace antifurto;

int main(int, char*[])
{
    //std::this_thread::sleep_for(std::chrono::seconds(5));
    MonitorController controller;
    std::cout << "Monitoring started.\n"
                 "Press any key to stop...";
    //std::this_thread::sleep_for(std::chrono::hours(2));
    std::string dummy;
    std::cin >> dummy;
    return 0;
}

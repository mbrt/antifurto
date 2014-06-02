#include "WhatsappNotifier.hpp"

#include <iostream>

using namespace antifurto;

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "usage: " << argv[0]
                  << " <base-dir> <config-file> <tel-number>\n";
        return 1;
    }

    const char* baseDir = argv[1];
    const char* configFile = argv[2];
    const char* number = argv[3];

    try {
        WhatsappNotifier notifier(baseDir, configFile);
        notifier.send(number, "Try message");
        return 0;
    }
    catch (std::exception& e) {
       std::cerr << "Exception: " << e.what() << std::endl;
       return 1;
    }
}

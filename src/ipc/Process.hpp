#pragma once

#include "Exception.hpp"
#include <string>
#include <sstream>

namespace antifurto {
namespace ipc {

class Process
{
public:
    Process(std::string exePath);
    int run(const std::string& args, const std::string& workDir);
    int run(const std::string& args);
    std::string getStdOut() const;

private:
    int runCommand(const std::string& command);

    std::string exePath_;
    std::ostringstream stdout_;
};

} // namespace ipc
} // namespace antifurto

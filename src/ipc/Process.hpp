#pragma once

#include "Exception.hpp"
#include <string>

namespace antifurto {
namespace ipc {

class Process
{
public:
    Process() {}
    Process(std::string exePath);
    int run(const std::string& args, const std::string& workDir);
    int run(const std::string& args);
    std::string getStdOut() const;
    void kill() const;

private:
    int runCommand(const std::string& command);

    std::string exePath_;
    std::string stdout_;
};

} // namespace ipc
} // namespace antifurto

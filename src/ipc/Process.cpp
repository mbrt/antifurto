#include "Process.hpp"

#include <cstdio>
#include <sstream>
#include <array>

namespace antifurto {
namespace ipc {

Process::Process(std::string command)
    : exePath_(command)
{ }

int Process::run(const std::string& args, const std::string& workDir)
{
    std::ostringstream cmd;
    cmd << "cd \"" << workDir << "\"; " << exePath_
        << ' ' << args;
    return runCommand(cmd.str());
}

int Process::run(const std::string& args)
{
    std::ostringstream cmd;
    cmd << exePath_ << ' ' << args;
    return runCommand(cmd.str());
}

std::string Process::getStdOut() const
{
    return stdout_;
}

void Process::kill() const
{
    throw Exception("Kill not implemented");
}


int Process::runCommand(const std::string& command)
{
    FILE* proc = ::popen(command.c_str(), "r");
    if (!proc)
        throw Exception("Failed to run process");

    std::stringstream outStream;
    std::array<char, 512> buffer;
    std::size_t len;
    while ((len = ::fread(&buffer[0], 1, buffer.size() - 1, proc)) > 0) {
        buffer[len] = '\0';
        outStream << &buffer[0];
    }
    stdout_ = outStream.str();

    return ::pclose(proc);
}

} // ipc
} // antifurto

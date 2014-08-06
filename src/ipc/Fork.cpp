#include "Fork.hpp"
#include "Exception.hpp"
#include "../text/ToString.hpp"

#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define USE_WAITID

namespace antifurto {
namespace ipc {

ChildProcess forkAndCall(std::function<int()> f)
{
    pid_t pid = ::fork();
    if (pid == 0) {
        // child
        int result = f();
        ::exit(result);
    }
    else if (pid > 0) {
        // parent
        return ChildProcess{pid};
    }
    else
        // failure
        throw Exception("Fork failed");
}

ChildProcess::ChildProcess(pid_t pid)
    : pid_(pid)
{ }

ChildProcess::ChildProcess(ChildProcess&& other)
{
    std::swap(pid_, other.pid_);
}

ChildProcess& ChildProcess::operator =(ChildProcess&& other)
{
    std::swap(pid_, other.pid_);
    return *this;
}

ChildProcess::~ChildProcess()
{
    try {
        if (pid_ > 0)
            wait();
    }
    catch (...) { }
}

void ChildProcess::kill(int signal)
{
    ::kill(pid_, signal);
}

int ChildProcess::wait()
{
#if defined(USE_WAITID)
    ::siginfo_t info;
    std::memset(&info, 0, sizeof(::siginfo_t));
    if (::waitid(P_PID, pid_, &info, WEXITED) == -1)
        throw Exception(text::toString("wait failed. errno: ", errno));
    pid_ = 0;
    return info.si_status;
#else
    int status;
    ::waitpid(pid_, &status, 0);
    return WEXITSTATUS(status);
#endif
}

} // namespace ipc
} // namespace antifurto

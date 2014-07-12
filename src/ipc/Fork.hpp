#pragma once

#include <functional>
#include <sched.h>

namespace antifurto {
namespace ipc {

/// This class represents a child process
class ChildProcess
{
public:
    explicit ChildProcess(pid_t pid = 0);
    ChildProcess(ChildProcess&& );
    ChildProcess& operator =(ChildProcess&& );
    ~ChildProcess();

    pid_t getPid() const { return pid_; }
    void kill(int signal = 9);
    int wait();

private:
    pid_t pid_;
};

/// This function fork the process, call the function in the child process,
/// wait for completion and returns the function return value.
ChildProcess forkAndCall(std::function<int()> f);

} // namespace ipc
} // namespace antifurto

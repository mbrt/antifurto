#include "Fork.hpp"
#include "Exception.hpp"

#include <unistd.h>
#include <sys/wait.h>

namespace antifurto {
namespace ipc {

int forkAndCall(std::function<int()> f)
{
    pid_t pid = ::fork();
    if (pid == 0) {
        // child
        int result = f();
        ::exit(result);
    }
    else if (pid > 0) {
        // parent
        int status;
        ::waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    else
        // failure
        throw Exception("Fork failed");
}

} // namespace ipc
} // namespace antifurto

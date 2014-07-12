#pragma once

#include <functional>

namespace antifurto {
namespace ipc {

/// This function fork the process, call the function in the child process,
/// wait for completion and returns the function return value.
int forkAndCall(std::function<int()> f);

} // namespace ipc
} // namespace antifurto


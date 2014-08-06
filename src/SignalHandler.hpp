#pragma once

#include <vector>

namespace antifurto {

// fwd references
namespace ipc {
class PosixSignalHandler;
}
class Antifurto;


/// This class is responsible to handle signals coming from other processes
/// and take the corresponding action
class SignalHandler
{
public:
    using SignalList = std::vector<int>;
    static SignalList getSignalsNeeded();

    SignalHandler(ipc::PosixSignalHandler& handler, Antifurto& controller);

private:
    void onSignal(int signal);

    ipc::PosixSignalHandler& handler_;
    Antifurto& controller_;
};

} // namespace antifurto

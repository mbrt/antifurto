#pragma once

#include <chrono>
#include <thread>

namespace antifurto {
namespace meta {

/// This class ensures that whenever you call the checkpoint,
/// the function retuns only when the predefined time has passed
/// since the last call
template <typename Resolution, typename Clock>
class Metronome
{
public:
    Metronome(Resolution cycle)
        : cycleTime_(cycle)
        , prevCheckpoint_(Clock::now())
    { }

    void checkpoint()
    {
        using namespace std::chrono;
        typename Clock::time_point now = Clock::now();
        Resolution elapsed = duration_cast<Resolution>(now - prevCheckpoint_);
        Resolution remaining = duration_cast<Resolution>(cycleTime_ - elapsed);
        if (remaining > Resolution::zero())
            std::this_thread::sleep_for(remaining);
        prevCheckpoint_ = Clock::now();
    }

private:
    Resolution cycleTime_;
    typename Clock::time_point prevCheckpoint_;
};

using DefaultMetronome =
    Metronome<std::chrono::milliseconds, std::chrono::steady_clock>;

} // namespace meta
} // namespace antifurto

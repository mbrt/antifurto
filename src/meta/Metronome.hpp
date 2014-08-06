#pragma once

#include <chrono>
#include <thread>
#include <mutex>

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

    void setCycleTime(Resolution cycle)
    {
        std::lock_guard<std::mutex> lock(m_);
        cycleTime_ = cycle;
    }

    void checkpoint()
    {
        Resolution remaining = getRemainingTime();
        if (remaining > Resolution::zero())
            std::this_thread::sleep_for(remaining);
        prevCheckpoint_ = Clock::now();
    }

private:
    Resolution getRemainingTime()
    {
        using namespace std::chrono;
        typename Clock::time_point now = Clock::now();
        Resolution elapsed = duration_cast<Resolution>(now - prevCheckpoint_);

        std::lock_guard<std::mutex> lock(m_);
        return duration_cast<Resolution>(cycleTime_ - elapsed);
    }

    Resolution cycleTime_;
    typename Clock::time_point prevCheckpoint_;
    std::mutex m_;
};

using DefaultMetronome =
    Metronome<std::chrono::milliseconds, std::chrono::steady_clock>;

} // namespace meta
} // namespace antifurto

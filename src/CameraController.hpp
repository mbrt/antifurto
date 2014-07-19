#pragma once

#include "StaticConfig.hpp"
#include "meta/Observer.hpp"
#include "meta/Metronome.hpp"

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace antifurto {

/// This class is responsible to take pictures at a give rate
class CameraController
{
public:
    using Subject = meta::Subject<const Picture&>;
    using Observer = Subject::Observer;
    using Registration = Subject::Registration;
    using Period = std::chrono::milliseconds;

    CameraController();
    ~CameraController();

    /// Set the rate at wich the pictures has to be taken
    void setDesiredPeriod(Registration const& r, Period period);

    /// Add an observer to the pictures flow
    Registration addObserver(Observer observer, Period desiredPeriod);

private:
    void controllerLoop();
    void onObserverRemoved(Subject::Id id);

    void addToDesiredPeriods(Subject::Id id, Period period);
    void removeFromDesiredPeriods(Subject::Id id);
    void updatePeriodFromDesired();

    struct PeriodItem {
        Period period;
        Subject::Id id;

        PeriodItem(Period p, Subject::Id id)
            : period(p), id(id) { }
    };

    using DesiredPeriods = std::vector<PeriodItem>;

    config::Camera camera_;
    meta::DefaultMetronome metronome_;
    Subject subject_;
    bool done_ = false;
    std::mutex cvm_;
    std::condition_variable cv_;
    std::mutex desiredPeriodsM_;
    DesiredPeriods desiredPeriods_;
    std::thread thread_;
};

} // namespace antifurto

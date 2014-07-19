#pragma once

#include "StaticConfig.hpp"
#include "meta/Observer.hpp"
#include "meta/Metronome.hpp"

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace antifurto {

/// This class is responsible to take pictures at a give rate
class CameraController
{
public:
    using Subject = meta::Subject<const Picture&>;
    using Observer = Subject::Observer;
    using Registration = Subject::Registration;

    CameraController();
    ~CameraController();

    /// Set the rate at wich the pictures has to be taken
    void setPeriod(std::chrono::milliseconds period);

    /// Add an observer to the pictures flow
    Registration addObserver(Observer observer);

private:
    void controllerLoop();

    config::Camera camera_;
    meta::DefaultMetronome metronome_;
    Subject subject_;
    bool done_ = false;
    std::mutex m_;
    std::condition_variable cv_;
    std::thread thread_;
};

} // namespace antifurto

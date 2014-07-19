#pragma once

#include "StaticConfig.hpp"
#include "meta/Observer.hpp"
#include "meta/Metronome.hpp"

#include <chrono>

namespace antifurto {

/// This class is responsible to take pictures at a give rate
class CameraController
{
public:
    using Subject = meta::Subject<const Picture&>;
    using Observer = Subject::Observer;
    using Registration = Subject::Registration;

    CameraController();

    /// Set the rate at wich the pictures has to be taken
    void setPeriod(std::chrono::milliseconds period);

    /// Add an observer to the pictures flow
    Registration addObserver(Observer observer);

private:
    config::Camera camera_;
    meta::DefaultMetronome metronome_;
};

} // namespace antifurto

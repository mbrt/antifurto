#pragma once

#include "Picture.hpp"
#include "Exception.hpp"

#include <opencv2/core/core.hpp>
#include <functional>


namespace antifurto {

/// This class detect motion and fires alarm events
class MotionDetector
{
public:
    MotionDetector(unsigned int numPreAlarmMotions = 4,
                   unsigned int numNoMotionToStopAlarm = 20);
    void examinePicture(Picture const& p);

    // observer subject interface
    enum class State {
        NO_ALARM, PRE_ALARM, ALARM
    };
    using Observer = std::function<void(State)>;
    void AddObserver(Observer o);

private:
    unsigned int countMotionPixels();
    void onMotionDetected();
    void onNoMotion();
    void notifyObservers();

    using ObserverList = std::vector<Observer>;

    cv::Mat prevDiff_;
    cv::Mat motion_;
    Picture curr_;
    State state_;
    unsigned int numPreAlarmMotions_;
    unsigned int numNoMotionToStopAlarm_;
    unsigned int consecutiveMotions_;
    unsigned int consecutiveNoMotion_;
    ObserverList observerList_;
};

} // namespace antifurto

#pragma once

#include "Picture.hpp"
#include "Exception.hpp"

#include <opencv2/core/core.hpp>


namespace antifurto {

/// This class detect motion and fires alarm events
class MotionDetector
{
public:
    MotionDetector(unsigned int numPreAlarmMotions);
    void examinePicture(Picture const& p);

private:
    unsigned int countMotionPixels();
    void onMotionDetected();
    void onNoMotion();

    enum class State {
        NO_ALARM, PRE_ALARM, ALARM
    };

    cv::Mat prevDiff_;
    cv::Mat motion_;
    Picture curr_;
    State state_;
    unsigned int numPreAlarmMotions_;
};

} // namespace antifurto

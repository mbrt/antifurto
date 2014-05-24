#include "MotionDetector.hpp"

namespace antifurto {
namespace { // anon

constexpr int MIN_PIXEL_DIFF = 35;
constexpr int MIN_MOTION_PIXELS = 60;

} // anon namespace


MotionDetector::MotionDetector(unsigned int numPreAlarmMotions)
    : state_(State::NO_ALARM)
    , numPreAlarmMotions_(numPreAlarmMotions)
{ }

void MotionDetector::examinePicture(const Picture &p)
{
    // initialization with first pictures
    if (static_cast<cv::Mat>(curr_).empty())
        curr_ = p;
    else if (prevDiff_.empty())
        cv::absdiff(curr_, p, prevDiff_);
    else
    {
        // main check loop
        cv::absdiff(curr_, p, motion_);
        cv::bitwise_and(prevDiff_, motion_, motion_);
        if (countMotionPixels() >= MIN_MOTION_PIXELS)
            onMotionDetected();
        else
            onNoMotion();
    }
}

unsigned int MotionDetector::countMotionPixels()
{
    unsigned int ndiff = 0;
    for (int row = 0; row < motion_.rows; ++row)
    {
        uchar* p = motion_.ptr(row);
        ndiff += std::count_if(p, p + motion_.cols, [](uchar a){ return a > MIN_PIXEL_DIFF; });
    }
    return ndiff;
}

void MotionDetector::onMotionDetected()
{
    switch (state_)
    {
    case State::NO_ALARM:
        {
            state_ = State::PRE_ALARM;
            // notify observers
        }
        break;
    default:
        throw UnexpectedException("unexpected motion detector state");
    }
}

void MotionDetector::onNoMotion()
{

}

} // namespace antifurto

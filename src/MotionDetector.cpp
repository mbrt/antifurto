#include "MotionDetector.hpp"

namespace antifurto {
namespace { // anon

constexpr int MIN_PIXEL_DIFF = 35;
constexpr int MIN_MOTION_PIXELS = 60;

} // anon namespace


MotionDetector::
MotionDetector(unsigned int numPreAlarmMotions,
               unsigned int numNoMotionToStopAlarm)
    : state_(State::NO_ALARM)
    , numPreAlarmMotions_(numPreAlarmMotions)
    , numNoMotionToStopAlarm_(numNoMotionToStopAlarm)
    , consecutiveMotions_(0)
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

void MotionDetector::AddObserver(MotionDetector::Observer o)
{
    observerList_.push_back(o);
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
    State prev = state_;
    consecutiveNoMotion_ = 0;

    switch (state_)
    {
    case State::NO_ALARM:
        state_ = State::PRE_ALARM;
        break;
    case State::PRE_ALARM:
        if (++consecutiveMotions_ >= numPreAlarmMotions_)
            state_ = State::ALARM;
        break;
    case State::ALARM:
        break;
    default:
        throw UnexpectedException("unexpected motion detector state");
    }

    if (prev != state_)
        notifyObservers();
}

void MotionDetector::onNoMotion()
{
    consecutiveMotions_ = 0;
    if (state_ == State::ALARM
            && ++consecutiveNoMotion_ >= numNoMotionToStopAlarm_)
    {
        state_ = State::NO_ALARM;
        notifyObservers();
    }
}

void MotionDetector::notifyObservers()
{
    for (auto& o : observerList_)
        o(state_);
}

} // namespace antifurto

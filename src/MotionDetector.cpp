#include "MotionDetector.hpp"

#include <iostream>


namespace antifurto {
namespace { // anon

constexpr int MIN_PIXEL_DIFF = 35;
constexpr int MIN_MOTION_PIXELS = 60;

} // anon namespace


MotionDetector::
MotionDetector(unsigned int numPreAlarmMotions, unsigned int numNoMotionToConsider,
               unsigned int numNoMotionToStopAlarm)
    : state_(State::NO_ALARM)
    , numPreAlarmMotions_(numPreAlarmMotions)
    , numNoMotionToConsider_(numNoMotionToConsider)
    , numNoMotionToStopAlarm_(numNoMotionToStopAlarm)
    , consecutiveMotions_(0)
{
    if (numNoMotionToConsider_ >= numNoMotionToStopAlarm_)
        throw UnexpectedException("Invalid options in MotionDetector");
}

void MotionDetector::examinePicture(const Picture &p)
{
    // initialization with first pictures
    if (static_cast<cv::Mat>(curr_).empty())
        curr_ = p;
    else if (prevDiff_.empty())
    {
        cv::absdiff(curr_, p, prevDiff_);
        curr_ = p;
    }
    else
    {
        // main check loop
        cv::absdiff(curr_, p, currDiff_);
        cv::bitwise_and(prevDiff_, currDiff_, motion_);
        if (motionHappened())
            onMotionDetected();
        else
            onNoMotion();
        // save
        std::swap(prevDiff_, currDiff_);
        curr_ = p;
    }
}

void MotionDetector::addObserver(MotionDetector::Observer o)
{
    observerList_.push_back(o);
}

void MotionDetector::clearObservers()
{
    observerList_.clear();
}

void MotionDetector::reset()
{
    curr_ = cv::Mat();
    prevDiff_ = cv::Mat();
}

void MotionDetector::dumpState(std::ostream& out) const
{
    out << state_;
}


bool MotionDetector::motionHappened()
{
    int motion = 0;
    const int rows = motion_.rows;
    const int cols = motion_.cols;
    uchar* p = motion_.data;
    for (int i = 0; i < rows * cols && motion < MIN_MOTION_PIXELS; ++i) {
        if (*p++ > MIN_PIXEL_DIFF)
            ++motion;
    }
    return motion >= MIN_MOTION_PIXELS;
}

void MotionDetector::onMotionDetected()
{
    State prev = state_;
    consecutiveNoMotion_ = 0;
    ++consecutiveMotions_;

    switch (state_) {
    case State::NO_ALARM:
        state_ = State::PRE_ALARM;
        break;
    case State::PRE_ALARM:
        if (consecutiveMotions_ >= numPreAlarmMotions_)
            state_ = State::ALARM;
        break;
    case State::ALARM:
        break;
    case State::NO_MOTION:
        state_ = State::ALARM;
        break;
    }

    if (prev != state_)
        notifyObservers();
}

void MotionDetector::onNoMotion()
{
    State prev = state_;
    consecutiveMotions_ = 0;
    ++consecutiveNoMotion_;

    switch (state_) {
    case State::NO_ALARM:
        break;
    case State::PRE_ALARM:
        state_ = State::NO_ALARM;
        break;
    case State::ALARM:
        if (consecutiveNoMotion_ >= numNoMotionToConsider_)
            state_ = State::NO_MOTION;
        break;
    case State::NO_MOTION:
        if (consecutiveNoMotion_ >= numNoMotionToStopAlarm_)
            state_ = State::NO_ALARM;
        break;
    }

    if (prev != state_)
        notifyObservers();
}

void MotionDetector::notifyObservers()
{
    for (auto& o : observerList_)
        o(state_);
}

} // namespace antifurto


namespace std {

ostream& operator <<(ostream& o, antifurto::MotionDetector::State s)
{
    using State = antifurto::MotionDetector::State;

    switch (s) {
    case State::ALARM:
        o << "Alarm";
        break;
    case State::NO_ALARM:
        o << "No alarm";
        break;
    case State::PRE_ALARM:
        o << "Pre alarm";
        break;
    case State::NO_MOTION:
        o << "No motion";
        break;
    }
    return o;
}

} // namespace std

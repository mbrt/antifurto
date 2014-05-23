#include "MotionDetector.hpp"

namespace antifurto {
namespace { // anon

constexpr int MIN_PIXEL_DIFF = 35;
constexpr int MIN_MOTION_PIXELS = 60;

} // anon namespace

void MotionDetector::examinePicture(const Picture &p)
{
    if (static_cast<cv::Mat>(curr_).empty())
        curr_ = p;
    else if (prevDiff_.empty())
        cv::absdiff(curr_, p, prevDiff_);
    else
    {
        cv::absdiff(curr_, p, motion_);
        cv::bitwise_and(prevDiff_, motion_, motion_);
        if (countMotionPixels() >= MIN_MOTION_PIXELS)
            onMotionDetected();
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

}

} // namespace antifurto

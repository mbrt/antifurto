#pragma once

#include "Picture.hpp"
#include "Exception.hpp"

#include <opencv2/core/core.hpp>


namespace antifurto {

/// This class detect motion and fires alarm events
class MotionDetector
{
public:
    void examinePicture(Picture const& p);

private:
    unsigned int countMotionPixels();
    void onMotionDetected();

    cv::Mat prevDiff_;
    cv::Mat motion_;
    Picture curr_;
};

} // namespace antifurto

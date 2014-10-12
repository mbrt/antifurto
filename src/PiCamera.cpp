#include "PiCamera.hpp"
#include "text/ToString.hpp"

#include <picam.h>

namespace antifurto {

PiCamera::PiCamera(int width, int height)
    : width_(width), height_(height)
    , capture_(::picam_start_camera(width, height, 10, 1, false),
               &::picam_stop_camera)
{
    if (!capture_)
        throw CameraException(text::toString(
            "Cannot initialize camera. Error: ",
            ::picam_get_last_error()));
}

void PiCamera::takePicture(Picture& p)
{
    resizePicture(p);
    const void* frameData;
    int bufferSize;
    ::picam_begin_read_frame(capture_.get(), 0, frameData, bufferSize);
    ::memcpy(static_cast<cv::Mat&>(p).data, frameData, width_ * height_);
    ::picam_end_read_frame(capture_.get(), 0);
}

void PiCamera::resizePicture(Picture& p) const
{
    cv::Mat& mat = static_cast<cv::Mat&>(p);
    if (mat.rows != height_ || mat.cols != width_)
        mat.create(height_, width_, CV_8UC1);
}

} // namespace antifurto

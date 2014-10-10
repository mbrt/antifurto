#include "PiCamera.hpp"
#include <picam.h>

namespace antifurto {

PiCamera::PiCamera(int width, int height)
    : width_(width), height_(height)
    , capture_(::StartCamera(width, height, 10, 1, false), &::StopCamera)
{
    if (!capture_)
        throw CameraException("Cannot initialize camera");
}

void PiCamera::takePicture(Picture& p)
{
    resizePicture(p);
    const void* frameData;
    int bufferSize;
    ::BeginReadFrame(capture_.get(), 0, frameData, bufferSize);
    ::memcpy(static_cast<cv::Mat&>(p).data, frameData, width_ * height_);
    ::EndReadFrame(capture_.get(), 0);
}

void PiCamera::resizePicture(Picture& p) const
{
    cv::Mat& mat = static_cast<cv::Mat&>(p);
    if (mat.rows != height_ || mat.cols != width_)
        mat.create(height_, width_, CV_8UC1);
}

} // namespace antifurto

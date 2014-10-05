#include "PiCamera.hpp"
#include <picam.h>

namespace antifurto {

PiCamera::PiCamera()
    : capture_(StartCamera(640, 480, 10, 1, false))
{
    if (!capture_)
        throw CameraException("Cannot initialize camera");
//    ::raspiCamCvSetCapturePropertyWidth(capture_, 640);
//    if (!capture_)
//        throw CameraException("Cannot initialize camera");
}

void PiCamera::takePicture(Picture& p)
{
//    frame_ = ::raspiCamCvQueryFrame(capture_);
//    p = frame_;
}

} // namespace antifurto

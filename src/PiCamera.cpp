#include "PiCamera.hpp"

namespace antifurto {

PiCamera::PiCamera()
    : capture_(::raspiCamCvCreateCameraCapture(0))
{
    ::raspiCamCvSetCapturePropertyWidth(capture_, 640);
    if (!capture_)
        throw CameraException("Cannot initialize camera");
}

void PiCamera::takePicture(Picture& p)
{
    frame_ = ::raspiCamCvQueryFrame(capture_);
    p = frame_;
}

} // namespace antifurto

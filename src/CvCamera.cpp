#include "CvCamera.hpp"


namespace antifurto {

CvCamera::CvCamera()
    : capture_(cvCaptureFromCAM(CV_CAP_ANY))
{
    if (!capture_)
        throw CameraException("Cannot initialize camera");
}

void CvCamera::takePicture(Picture& p)
{
    p = ::cvQueryFrame(capture_);
}

} // namespace antifurto

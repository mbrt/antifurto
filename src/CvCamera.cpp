#include "CvCamera.hpp"


namespace antifurto {

CvCamera::CvCamera()
    : capture(cvCaptureFromCAM(CV_CAP_ANY))
{
    if (!capture)
        throw CameraException("Cannot initialize camera");
}

void CvCamera::takePicture(Picture& p)
{
    p = ::cvQueryFrame(capture);
}

} // namespace antifurto

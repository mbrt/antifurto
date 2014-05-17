#include "CvCamera.hpp"


namespace antifurto {

CvCamera::CvCamera()
    : capture(cvCaptureFromCAM(CV_CAP_ANY))
{
    // TODO: Check not null
}

void CvCamera::takePicture(Picture& p)
{
    p = ::cvQueryFrame(capture);
}

} // namespace antifurto

#include "CvCamera.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/utility.hpp>


namespace antifurto {

CvCamera::CvCamera()
    : capture_(cvCaptureFromCAM(CV_CAP_ANY))
{
    if (!capture_)
        throw CameraException("Cannot initialize camera");
}

void CvCamera::takePicture(Picture& p)
{
  frame_ = cv::cvarrToMat(::cvQueryFrame(capture_));
    cv::cvtColor(frame_, p, CV_RGB2GRAY);
}

} // namespace antifurto

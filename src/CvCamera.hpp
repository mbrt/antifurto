#pragma once

#include "Picture.hpp"
#include "Camera.hpp"

#include <opencv2/opencv.hpp>
#include <memory>


namespace antifurto {

class CvCaptureRAII
{
public:
    CvCaptureRAII(CvCapture* capture)
        : capture(capture, &releaseCapture)
    { }

    operator CvCapture*() { return capture.get(); }
    operator CvCapture&() { return *capture; }
    operator CvCapture const&() const { return *capture; }

private:
    static void releaseCapture(CvCapture*& capture)
    { ::cvReleaseCapture(&capture); }

    using Ptr = std::unique_ptr<
        CvCapture,
        decltype(&releaseCapture)>;
    Ptr capture;
};


class CvCamera : public Camera
{
public:
    CvCamera();
    void takePicture(Picture& p) override;

private:
    CvCaptureRAII capture;
};

} // namespace antifurto

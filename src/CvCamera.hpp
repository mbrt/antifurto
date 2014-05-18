#pragma once

#include "Picture.hpp"
#include "Camera.hpp"
#include "Exception.hpp"

#include <opencv2/highgui/highgui_c.h>
#include <memory>


namespace antifurto {

/// RAII For CvCapture resource
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


/// Camera implementation that use CvCapture resource
class CvCamera : public Camera
{
public:
    CvCamera();
    void takePicture(Picture& p) override;

private:
    CvCaptureRAII capture;
};

struct CameraException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto

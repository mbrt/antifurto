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
        : capture_(capture, &releaseCapture)
    { }

    operator CvCapture*() { return capture_.get(); }
    operator CvCapture&() { return *capture_; }
    operator CvCapture const&() const { return *capture_; }

private:
    static void releaseCapture(CvCapture*& capture)
    { ::cvReleaseCapture(&capture); }

    using Ptr = std::unique_ptr<
        CvCapture,
        decltype(&releaseCapture)>;
    Ptr capture_;
};


/// Camera implementation that use CvCapture resource
class CvCamera : public Camera
{
public:
    CvCamera();
    void takePicture(Picture& p) override;

private:
    CvCaptureRAII capture_;
};

struct CameraException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto

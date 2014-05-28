#pragma once

#include "Picture.hpp"
#include "Camera.hpp"
#include "Exception.hpp"

#include <memory>
#include <RaspiCamCV.h>


namespace antifurto {

/// RAII For PiCapture resource
class PiCaptureRAII
{
public:
    PiCaptureRAII(RaspiCamCvCapture* capture)
        : capture_(capture, &releaseCapture)
    { }

    operator RaspiCamCvCapture*() { return capture_.get(); }
    operator RaspiCamCvCapture&() { return *capture_; }
    operator RaspiCamCvCapture const&() const { return *capture_; }

private:
    static void releaseCapture(RaspiCamCvCapture*& capture)
    { ::raspiCamCvReleaseCapture(&capture); }

    using Ptr = std::unique_ptr<
        RaspiCamCvCapture,
        decltype(&releaseCapture)>;
    Ptr capture_;
};


/// Camera implementation that use PiCapture resource
class PiCamera : public Camera
{
public:
    PiCamera();
    void takePicture(Picture& p) override;

private:
    PiCaptureRAII capture_;
    cv::Mat frame_;
};

} // namespace antifurto

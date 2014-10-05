#pragma once

#include "Picture.hpp"
#include "Camera.hpp"
#include "Exception.hpp"
#include "meta/SmartPtr.hpp"

#include <memory>

// fwd reference
class CCamera;

namespace antifurto {

/// Camera implementation that use PiCapture resource
class PiCamera : public Camera
{
public:
    PiCamera();
    void takePicture(Picture& p) override;

private:
    cv::Mat frame_;
    meta::ErasedUniquePtr<CCamera> capture_;
};

} // namespace antifurto

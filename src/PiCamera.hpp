#pragma once

#include "Picture.hpp"
#include "Camera.hpp"
#include "Exception.hpp"

#include <memory>

// fwd reference
class CCamera;

namespace antifurto {

/// Camera implementation that use PiCapture resource
class PiCamera : public Camera
{
public:
    PiCamera(int width = 640, int height = 480);
    void takePicture(Picture& p) override;

private:
    void resizePicture(Picture& p) const;

    int width_;
    int height_;
    std::unique_ptr<CCamera, void(*)(CCamera*)> capture_;
};

} // namespace antifurto

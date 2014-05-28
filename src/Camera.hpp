#pragma once

#include "Picture.hpp"
#include "Exception.hpp"

namespace antifurto {

class Camera
{
public:
    virtual void takePicture(Picture& p) = 0;
    virtual ~Camera() {}
};

struct CameraException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto

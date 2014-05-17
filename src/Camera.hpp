#pragma once

#include "Picture.hpp"


namespace antifurto {

class Camera
{
public:
    virtual void takePicture(Picture& p) = 0;
    virtual ~Camera() {}
};

} // namespace antifurto
